#!/usr/bin/env python3
"""
CPP-SQLite-ORM: A mini ORM for SQLite and C++
Generates CRUD operations from C++ struct definitions with annotations.
"""

import argparse
import os
import re
import subprocess
import sys
import tempfile
from dataclasses import dataclass, field
from datetime import datetime
from pathlib import Path
from typing import Any, Dict, List, Optional

import clang.cindex
from clang.cindex import CursorKind
from jinja2 import Template


class StructParser:
    """
    A parser for C++ struct definitions that extracts struct metadata,
    field information, and annotations.
    """

    def __init__(self, libclang_path: str = "/usr/lib/libclang.so"):
        """Initialize the parser with libclang library path."""
        try:
            clang.cindex.Config.set_library_file(libclang_path)
        except Exception as e:
            print(f"Warning: Could not set libclang path: {e}")

    @staticmethod
    def get_annotations(node: clang.cindex.Cursor) -> List[str]:
        """
        Extract annotations from a node.

        Args:
            node: The clang cursor to extract annotations from

        Returns:
            List of annotation display names
        """
        return [
            child.displayname
            for child in node.get_children()
            if child.kind == clang.cindex.CursorKind.ANNOTATE_ATTR
        ]

    def parse_file(
        self, file_path: str, compiler_args: Optional[List[str]] = None
    ) -> List[Dict[str, Any]]:
        """
        Parse a C++ header file and extract all struct definitions.

        Args:
            file_path: Path to the C++ header file to parse
            compiler_args: Additional compiler arguments for parsing

        Returns:
            List of dictionaries containing struct information
        """
        file_path = Path(file_path)
        if not file_path.exists():
            raise FileNotFoundError(f"File not found: {file_path}")

        # Default compiler arguments
        if compiler_args is None:
            compiler_args = [
                "-x",
                "c++",
                "-std=c++20",
                "-D__CODE_GENERATOR__",
            ]

        try:
            index = clang.cindex.Index.create()
            translation_unit = index.parse(str(file_path), args=compiler_args)
        except Exception as e:
            raise RuntimeError(f"Failed to parse file {file_path}: {e}")

        target_stem = file_path.stem
        structs = []

        for node in translation_unit.cursor.walk_preorder():
            # Skip nodes that don't belong to our target file
            if self._should_skip_node(node, target_stem):
                continue

            if (
                node.kind == CursorKind.STRUCT_DECL
                and node.is_definition()
                and node.spelling
            ):  # Skip anonymous structs

                struct_info = self._parse_struct_node(node)
                structs.append(struct_info)

        return structs

    def _should_skip_node(self, node: clang.cindex.Cursor, target_stem: str) -> bool:
        """
        Determine if a node should be skipped during parsing.

        Args:
            node: The clang cursor to check
            target_stem: The stem of the target file

        Returns:
            True if the node should be skipped, False otherwise
        """
        if node.location.file is None:
            return True

        node_file_stem = Path(node.location.file.name).stem
        return node_file_stem != target_stem

    def _parse_struct_node(self, node: clang.cindex.Cursor) -> Dict[str, Any]:
        """
        Parse a single struct node and extract its information.

        Args:
            node: The struct cursor to parse

        Returns:
            Dictionary containing struct information
        """
        struct_info = {
            "name": node.spelling,
            "fields": [],
            "attributes": self.get_annotations(node),
            "location": {
                "file": node.location.file.name if node.location.file else None,
                "line": node.location.line,
                "column": node.location.column,
            },
        }

        for child in node.get_children():
            if child.kind == CursorKind.FIELD_DECL:
                field_info = self._parse_field_node(child)
                struct_info["fields"].append(field_info)

        return struct_info

    def _parse_field_node(self, node: clang.cindex.Cursor) -> Dict[str, Any]:
        """
        Parse a single field node and extract its information.

        Args:
            node: The field cursor to parse

        Returns:
            Dictionary containing field information
        """
        return {
            "name": node.spelling,
            "type": node.type.spelling,
            "attrs": self.get_annotations(node),
            "location": {
                "file": node.location.file.name if node.location.file else None,
                "line": node.location.line,
                "column": node.location.column,
            },
        }


class EntityType:
    TABLE = "Table"
    VIEW = "View"


@dataclass
class Field:
    name: str
    is_primary_key: bool = False
    is_foreign_key: bool = False
    is_auto_increment: bool = False
    is_readonly: bool = False
    is_ignored: bool = False
    original_type: Optional[str] = None
    annotations: List[str] = field(default_factory=list)


@dataclass
class Entity:
    name: str
    entity_type: str  # EntityType.TABLE or EntityType.VIEW
    target_table: str  # Extracted from table:name or view:name annotations
    fields: List[Field]
    ignored: bool = False
    namespace: str = ""  # Extract namespace from the original location
    original_struct: Optional[Dict[str, Any]] = None
    annotations: List[str] = field(default_factory=list)

    @property
    def Name(self):
        return self.name

    @property
    def Target(self):
        return self.target_table

    @property
    def Fields(self):
        return [f.name for f in self.fields]

    @property
    def FieldsPlaceholders(self):
        return ", ".join(["?" for _ in self.fields])

    @property
    def InsertFields(self) -> List[str]:
        return [
            f.name for f in self.fields if not f.is_auto_increment and not f.is_readonly
        ]

    @property
    def InsertFieldsPlaceholders(self) -> str:
        return ", ".join(
            ["?" for f in self.fields if not f.is_auto_increment and not f.is_readonly]
        )

    def getFieldByName(self, name: str) -> Optional[Field]:
        """Get a field by its name, returns None if not found"""
        for field in self.fields:
            if field.name == name:
                return field
        return None

    @property
    def PrimaryKeys(self):
        return [f.name for f in self.fields if f.is_primary_key]

    @property
    def HasPrimaryKeys(self):
        return any(f.is_primary_key for f in self.fields)

    @property
    def TypedNames(self):
        return [f"{f.original_type} {f.name}" for f in self.fields]

    @property
    def TypedPrimaryKeys(self):
        return [f"{f.original_type} {f.name}" for f in self.fields if f.is_primary_key]

    @property
    def PkParms(self):
        return [
            f"{self.getFieldByName(pk).original_type} {pk}" for pk in self.PrimaryKeys
        ]


class EntityParser:
    """
    Parser to convert C++ struct definitions into Entity objects
    with support for the specific macro annotations
    """

    def __init__(self):
        # Patterns for macro annotations
        self.table_pattern = re.compile(r'table:([^"]+)')
        self.view_pattern = re.compile(r'view:([^"]+)')
        self.ignore_pattern = re.compile(r"ignore")
        self.primary_key_patterns = [re.compile(r"pk")]
        self.foreign_key_patterns = [re.compile(r"fk")]
        self.auto_inc_pattern = re.compile(r"autoinc")
        self.readonly_pattern = re.compile(r"readonly")

    def parse_from_structs(self, structs: List[Dict[str, Any]]) -> List[Entity]:
        """
        Convert parsed C++ structs into Entity objects

        Args:
            structs: List of struct dictionaries from the clang parser

        Returns:
            List of Entity objects
        """
        entities = []

        for struct in structs:
            entity = self._parse_single_entity(struct)
            if entity:
                entities.append(entity)

        return entities

    def _parse_single_entity(self, struct: Dict[str, Any]) -> Optional[Entity]:
        """
        Parse a single struct into an Entity

        Args:
            struct: Struct dictionary from clang parser

        Returns:
            Entity object or None if the struct should be ignored
        """
        # Extract namespace from the struct's location if available
        namespace = self._extract_namespace(struct)

        # Check if this entity should be ignored
        if self._should_ignore_entity(struct):
            return Entity(
                name=struct["name"],
                entity_type=EntityType.TABLE,
                target_table="",
                fields=[],
                ignored=True,
                namespace=namespace,
                original_struct=struct,
            )

        # Determine entity type and extract target table/view name
        entity_type, target_name = self._extract_entity_type_and_target(struct)

        if not target_name:
            print(f"Warning: Struct '{struct['name']}' has no TABLE or VIEW annotation")
            return None

        # Parse fields
        fields = self._parse_fields(struct.get("fields", []))

        return Entity(
            name=struct["name"],
            entity_type=entity_type,
            target_table=target_name,
            fields=fields,
            ignored=False,
            namespace=namespace,
            original_struct=struct,
            annotations=struct.get("attributes", []),
        )

    def _extract_namespace(self, struct: Dict[str, Any]) -> str:
        """
        Extract namespace from struct location or other available data

        Args:
            struct: Struct dictionary

        Returns:
            Namespace string
        """
        # Try to get namespace from location or other metadata
        # This might need adjustment based on how clang provides this info
        if "location" in struct and struct["location"].get("file"):
            # You might need to parse the file content to get namespace
            # For now, return empty string
            return ""
        return ""

    def _should_ignore_entity(self, struct: Dict[str, Any]) -> bool:
        """
        Check if an entity should be ignored based on IGNORE annotation

        Args:
            struct: Struct dictionary

        Returns:
            True if the entity should be ignored
        """
        for annotation in struct.get("attributes", []):
            if self.ignore_pattern.search(annotation):
                return True
        return False

    def _extract_entity_type_and_target(
        self, struct: Dict[str, Any]
    ) -> tuple[str, str]:
        """
        Extract entity type (Table/View) and target name from annotations

        Args:
            struct: Struct dictionary

        Returns:
            Tuple of (entity_type, target_name)
        """
        for annotation in struct.get("attributes", []):
            # Check for TABLE annotation
            table_match = self.table_pattern.search(annotation)
            if table_match:
                table_name = table_match.group(1).strip()
                # Remove any quotes if present
                table_name = table_name.replace('"', "").replace("'", "")
                return EntityType.TABLE, table_name

            # Check for VIEW annotation
            view_match = self.view_pattern.search(annotation)
            if view_match:
                view_name = view_match.group(1).strip()
                view_name = view_name.replace('"', "").replace("'", "")
                return EntityType.VIEW, view_name

        return EntityType.TABLE, ""

    def _parse_fields(self, fields_data: List[Dict[str, Any]]) -> List[Field]:
        """
        Parse field data into Field objects with support for all macro annotations

        Args:
            fields_data: List of field dictionaries from clang parser

        Returns:
            List of Field objects
        """
        fields = []

        for field_data in fields_data:
            # Check if the field should be ignored
            if self._should_ignore_field(field_data):
                continue

            field = Field(
                name=field_data["name"],
                original_type=field_data["type"],
                annotations=field_data.get("attrs", []),
            )

            # Set field properties based on annotations
            field.is_primary_key = self._is_primary_key(field_data)
            field.is_foreign_key = self._is_foreign_key(field_data)
            field.is_auto_increment = self._is_auto_increment(field_data)
            field.is_readonly = self._is_readonly(field_data)
            field.is_ignored = self._should_ignore_field(field_data)

            fields.append(field)

        return fields

    def _should_ignore_field(self, field_data: Dict[str, Any]) -> bool:
        """Check if a field has IGNORE annotation"""
        for annotation in field_data.get("attrs", []):
            if self.ignore_pattern.search(annotation):
                return True
        return False

    def _is_primary_key(self, field_data: Dict[str, Any]) -> bool:
        """Check if a field has PK annotation"""
        for annotation in field_data.get("attrs", []):
            for pattern in self.primary_key_patterns:
                if pattern.search(annotation):
                    return True
        return False

    def _is_foreign_key(self, field_data: Dict[str, Any]) -> bool:
        """Check if a field has FK annotation"""
        for annotation in field_data.get("attrs", []):
            for pattern in self.foreign_key_patterns:
                if pattern.search(annotation):
                    return True
        return False

    def _is_auto_increment(self, field_data: Dict[str, Any]) -> bool:
        """Check if a field has AUTOINC annotation"""
        for annotation in field_data.get("attrs", []):
            if self.auto_inc_pattern.search(annotation):
                return True
        return False

    def _is_readonly(self, field_data: Dict[str, Any]) -> bool:
        """Check if a field has READONLY annotation"""
        for annotation in field_data.get("attrs", []):
            if self.readonly_pattern.search(annotation):
                return True
        return False


# Enhanced utility functions
def filter_entities_by_type(entities: List[Entity], entity_type: str) -> List[Entity]:
    """Filter active entities by type"""
    return [
        entity
        for entity in entities
        if not entity.ignored and entity.entity_type == entity_type
    ]


def get_primary_keys(entity: Entity) -> List[Field]:
    """Get all primary key fields from an entity"""
    return [field for field in entity.fields if field.is_primary_key]


def get_foreign_keys(entity: Entity) -> List[Field]:
    """Get all foreign key fields from an entity"""
    return [field for field in entity.fields if field.is_foreign_key]


def get_auto_increment_fields(entity: Entity) -> List[Field]:
    """Get all auto-increment fields from an entity"""
    return [field for field in entity.fields if field.is_auto_increment]


def get_readonly_fields(entity: Entity) -> List[Field]:
    """Get all readonly fields from an entity"""
    return [field for field in entity.fields if field.is_readonly]


# Complete workflow integration
def parse_entities_from_file(file_path: str) -> List[Entity]:
    """
    Complete workflow: Parse C++ file and convert to Entity objects
    """

    struct_parser = StructParser()
    entity_parser = EntityParser()

    # Parse structs from file
    structs = struct_parser.parse_file(file_path)

    # Convert to entities
    entities = entity_parser.parse_from_structs(structs)

    return entities


def format_with_clangformat(code: str, style: str = "file") -> str:
    """
    Format C++ code using clang-format
    Styles: LLVM, Google, Chromium, Mozilla, WebKit, or 'file' to use .clang-format
    """
    try:
        with tempfile.NamedTemporaryFile(mode="w", suffix=".cpp", delete=False) as f:
            f.write(code)
            temp_file = f.name

        # Run clang-format
        result = subprocess.run(
            ["clang-format", f"-style={style}", temp_file],
            capture_output=True,
            text=True,
            check=True,
        )

        os.unlink(temp_file)
        return result.stdout

    except subprocess.CalledProcessError as e:
        print(f"clang-format error: {e}")
        return code
    except FileNotFoundError:
        print("clang-format not found. Please install it.")
        return code


def apply(template_name, entity):
    try:
        with open(template_name, "r") as file:
            template_content = file.read()

        return Template(template_content).render(entity=entity)

    except FileNotFoundError:
        raise ValueError(f"Template file not found: {template_name}")
    except Exception as e:
        raise ValueError(
            f"Error processing template {template_name}: {str(e)} Entity: {entity.Name}"
        )


def generate_crud_header(entities, source_file: str, namespace: str = "data"):
    """Generate complete CRUD header file with operations for both tables and views"""

    # Separate entities by type
    tables = [
        e for e in entities if not e.ignored and e.entity_type == EntityType.TABLE
    ]
    views = [e for e in entities if not e.ignored and e.entity_type == EntityType.VIEW]
    all_entities = tables + views

    # Generate operations
    insert_functions = {}
    select_all_functions = {}
    select_by_id_functions = {}
    update_functions = {}
    delete_functions = {}

    for entity in all_entities:
        if (
            entity.entity_type == EntityType.TABLE
            or entity.entity_type == EntityType.VIEW
        ):
            select_all_functions[entity.name] = apply(
                "templates/select-all.jinja", entity
            )
            if entity.HasPrimaryKeys:
                select_by_id_functions[entity.name] = apply(
                    "templates/select.jinja", entity
                )
                if entity.entity_type == EntityType.TABLE:
                    update_functions[entity.name] = apply(
                        "templates/update.jinja", entity
                    )
                    delete_functions[entity.name] = apply(
                        "templates/delete.jinja", entity
                    )
                    insert_functions[entity.name] = apply(
                        "templates/insert.jinja", entity
                    )

    # Load master template
    tmpl = Template(open("templates/master.jinja").read())

    return tmpl.render(
        timestamp=datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
        source_file=source_file,
        namespace=namespace,
        entities=all_entities,
        tables=tables,
        views=views,
        insert_functions=insert_functions,
        select_all_functions=select_all_functions,
        select_by_id_functions=select_by_id_functions,
        update_functions=update_functions,
        delete_functions=delete_functions,
    )


def generate_complete_crud(
    entities, source_file: str, output_file: str = "output.hpp", namespace: str = "data"
):
    """Generate and save complete CRUD header file with tables and views"""

    try:
        header_content = generate_crud_header(entities, source_file, namespace)
        header_content = format_with_clangformat(header_content)

        with open(output_file, "w") as f:
            f.write(header_content)

        # Print summary
        tables = [
            e for e in entities if not e.ignored and e.entity_type == EntityType.TABLE
        ]
        views = [
            e for e in entities if not e.ignored and e.entity_type == EntityType.VIEW
        ]

        print(f"Successfully generated: {output_file}")
        print(f"Tables processed: {len(tables)}")
        print(f"Views processed: {len(views)}")

        print("\nGenerated operations:")
        for entity in tables:
            pks = entity.PrimaryKeys
            ops = "INSERT, SELECT_ALL" + (
                f", SELECT_BY_ID, UPDATE, DELETE" if pks else ""
            )
            print(f"  TABLE {entity.name}: {ops}")

        for entity in views:
            pks = entity.PrimaryKeys
            ops = "SELECT_ALL" + (f", SELECT_BY_ID" if pks else "")
            print(f"  VIEW  {entity.name}: {ops}")

        return True

    except Exception as e:
        print(f"Error generating CRUD header: {e}")
        return False


def main():
    """Main function with command line argument support."""

    parser = argparse.ArgumentParser(
        description="CPP-SQLite-ORM: Generate CRUD operations from C++ struct definitions",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s entities.h
  %(prog)s entities.h -o crud_operations.hpp
  %(prog)s entities.h -n mynamespace -s Google
  %(prog)s entities.h --libclang /usr/lib/llvm-14/lib/libclang.so
        """,
    )

    parser.add_argument(
        "input_file", help="Input C++ header file with struct definitions"
    )

    parser.add_argument(
        "-o",
        "--output",
        default="output.hpp",
        help="Output header file (default: output.hpp)",
    )

    parser.add_argument(
        "-n",
        "--namespace",
        default="data",
        help="Namespace for generated code (default: data)",
    )

    parser.add_argument(
        "-s",
        "--style",
        default="file",
        choices=["LLVM", "Google", "Chromium", "Mozilla", "WebKit", "file"],
        help="clang-format style (default: file)",
    )

    parser.add_argument(
        "--libclang",
        default="/usr/lib/libclang.so",
        help="Path to libclang shared library (default: /usr/lib/libclang.so)",
    )

    parser.add_argument(
        "-v", "--verbose", action="store_true", help="Enable verbose output"
    )

    parser.add_argument("--version", action="version", version="CPP-SQLite-ORM 1.0.0")

    args = parser.parse_args()

    # Check if input file exists
    if not os.path.exists(args.input_file):
        print(f"Error: Input file '{args.input_file}' not found")
        sys.exit(1)

    # Check if templates directory exists
    if not os.path.exists("templates"):
        print("Error: 'templates' directory not found")
        print("Please ensure you're running from the project root directory")
        sys.exit(1)

    try:
        if args.verbose:
            print(f"Parsing {args.input_file}...")

        # Parse entities
        entities = parse_entities_from_file(args.input_file)

        if args.verbose:
            print(f"Found {len(entities)} entities")

        # Generate CRUD operations
        success = generate_complete_crud(
            entities=entities,
            source_file=args.input_file,
            output_file=args.output,
            namespace=args.namespace,
        )

        if success:
            if args.verbose:
                print("Code generation completed successfully")
            sys.exit(0)
        else:
            sys.exit(1)

    except FileNotFoundError as e:
        print(f"Error: {e}")
        print("Please ensure the file exists and the path is correct.")
        sys.exit(1)
    except RuntimeError as e:
        print(f"Parsing error: {e}")
        print("This might be due to missing include paths or compiler configuration.")
        sys.exit(1)
    except Exception as e:
        print(f"Unexpected error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
