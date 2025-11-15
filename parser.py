import re
import tempfile
from dataclasses import dataclass, field
from enum import Enum
from pathlib import Path
from typing import Any, Dict, List, Optional

import clang.cindex

MACRO = """
#ifdef __CODE_GENERATOR__
#define TABLE(name) __attribute__((annotate("table:" #name)))
#define VIEW(name) __attribute__((annotate("view:" #name)))
#define IGNORE __attribute__((annotate("ignore")))
#define PK __attribute__((annotate("pk")))
#define FK __attribute__((annotate("fk")))
#define AUTOINC __attribute__((annotate("autoinc")))
#define READONLY __attribute__((annotate("readonly")))
#define WIDTH(w) __attribute__((annotate("width:" #w)))
#else
#define TABLE(name)
#define VIEW(name)
#define IGNORE
#define PK
#define FK
#define AUTOINC
#define READONLY
#define WIDTH(w)
#endif
"""

COMPILER_ARGS = [
    "-x",
    "c++",
    "-std=c++14",
    "-D__CODE_GENERATOR__",
]

TABLE_RGX = re.compile(r"table:(.+)")
VIEW_RGX = re.compile(r"view:(.+)")
WIDTH_RGX = re.compile(r'width:([^"]+)')
IGNORE_RGX = re.compile(r"ignore")
PK_RGX = re.compile(r"pk")
FK_RGX = re.compile(r"fk")
AUTOINC_RGX = re.compile(r"autoinc")
READONLY_RGX = re.compile(r"readonly")


class EntityType(Enum):
    TABLE = "table"
    VIEW = "view"
    ENUM = "enum"


@dataclass
class Field:
    name: str
    is_primary_key: bool = False
    width: int = 20
    is_foreign_key: bool = False
    is_auto_increment: bool = False
    is_readonly: bool = False
    is_ignored: bool = False
    is_enum: bool = False
    original_type: Optional[str] = None
    # annotations: List[str] = field(default_factory=list)


@dataclass
class Entity:
    Name: str
    entity_type: EntityType
    Fields: List[Field] = field(default_factory=list)
    Target: Optional[str] = None
    is_ignored: bool = False
    namespace: Optional[str] = None


def print_field(field: Field, indent: int = 0) -> None:
    """Print a Field object with formatted output."""
    indent_str = " " * indent
    print(f"{indent_str}Field: {field.name}")
    print(f"{indent_str}  Type: {field.original_type or 'Not specified'}")
    print(f"{indent_str}  Width: {field.width}")

    # Print flags
    flags = []
    if field.is_primary_key:
        flags.append("Primary Key")
    if field.is_foreign_key:
        flags.append("Foreign Key")
    if field.is_auto_increment:
        flags.append("Auto Increment")
    if field.is_readonly:
        flags.append("Readonly")
    if field.is_ignored:
        flags.append("Ignored")
    if field.is_enum:
        flags.append("Enum")

    if flags:
        print(f"{indent_str}  Flags: {', '.join(flags)}")


def print_entity(entity: Entity, show_fields: bool = True) -> None:
    """Print an Entity object with formatted output."""
    print("=" * 60)
    print(f"ENTITY: {entity.Name}")
    print("=" * 60)
    print(f"Type: {entity.entity_type}")

    if entity.namespace:
        print(f"Namespace: {entity.namespace}")

    if entity.Target:
        print(f"Target: {entity.Target}")

    if entity.is_ignored:
        print("Status: IGNORED")

    print(f"Number of fields: {len(entity.Fields)}")
    print()

    if show_fields and entity.Fields:
        print("FIELDS:")
        print("-" * 40)
        for field_obj in entity.Fields:
            print_field(field_obj, indent=2)


def print_entities(entities: List[Entity], show_fields: bool = True) -> None:
    """Print multiple Entity objects."""
    for i, entity in enumerate(entities):
        if i > 0:
            print("\n" + "=" * 60 + "\n")
        print_entity(entity, show_fields)


# Alternative: Override __str__ methods for built-in printing
def __field_str__(self) -> str:
    """String representation of Field."""
    flags = []
    if self.is_primary_key:
        flags.append("PK")
    if self.is_foreign_key:
        flags.append("FK")
    if self.is_auto_increment:
        flags.append("AI")
    if self.is_readonly:
        flags.append("RO")
    if self.is_ignored:
        flags.append("IGN")
    if self.is_enum:
        flags.append("ENUM")

    flags_str = f" [{', '.join(flags)}]" if flags else ""
    type_str = f" ({self.original_type})" if self.original_type else ""
    return f"Field: {self.name}{type_str}{flags_str}"


def __entity_str__(self) -> str:
    """String representation of Entity."""
    fields_str = "\n  ".join(str(field) for field in self.Fields)
    return f"Entity: {self.Name} ({self.entity_type})\n  {fields_str}"


# Add the string methods to the classes (optional)
Field.__str__ = __field_str__
Entity.__str__ = __entity_str__


def combine_header_with_file(original_content: str) -> str:
    lines = original_content.split("\n")
    injection_point = 0

    # Find the last include statement or start of file
    for i, line in enumerate(lines):
        if line.strip().startswith("#include"):
            injection_point = i + 1
        elif line.strip() and not line.strip().startswith("#"):
            # Found non-preprocessor code, stop here
            break
    # Inject the ORM header include

    lines.insert(injection_point, f"// Injected ORM header content\n{MACRO}")
    combined_content = "\n".join(lines)
    return combined_content


def get_annotations(node: clang.cindex.Cursor) -> List[str]:
    return [
        child.displayname
        for child in node.get_children()
        if child.kind == clang.cindex.CursorKind.ANNOTATE_ATTR
    ]


def process_entity(
    node: Any, namespace: Optional[str], target_stem: str
) -> Optional[Entity]:
    entity_name = node.spelling
    entity = Entity(
        Name=entity_name,
        entity_type=EntityType.TABLE,
        namespace=namespace,
    )

    for attr in get_annotations(node):
        if match := TABLE_RGX.match(attr):
            entity.entity_type = EntityType.TABLE
            entity.Target = match.group(1)
            # entity.annotations.append(annotation)
        elif match := VIEW_RGX.match(attr):
            entity.entity_type = EntityType.VIEW
            entity.Target = match.group(1)
            # entity.annotations.append(annotation)
        elif IGNORE_RGX.match(attr):
            entity.is_ignored = True
            # entity.annotations.append(annotation)
    if not entity.Target:
        entity.Target = None
        entity.is_ignored = True

    for child in node.get_children():
        if child.kind == clang.cindex.CursorKind.FIELD_DECL:
            field_name = child.spelling
            field = Field(name=child.spelling, original_type=child.type.spelling)
            field.is_enum = (
                child.type.get_canonical().kind == clang.cindex.TypeKind.ENUM
            )

            for attr in get_annotations(child):
                if PK_RGX.match(attr):
                    field.is_primary_key = True
                elif FK_RGX.match(attr):
                    field.is_foreign_key = True
                elif AUTOINC_RGX.match(attr):
                    field.is_auto_increment = True
                elif READONLY_RGX.match(attr):
                    field.is_readonly = True
                elif IGNORE_RGX.match(attr):
                    field.is_ignored = True
                elif match := WIDTH_RGX.match(attr):
                    field.width = int(match.group(1))

            entity.Fields.append(field)

    return entity


def process_enum(node: Any, namespace: Optional[str]) -> Optional[Entity]:
    enum_name = node.spelling
    enum_entity = Entity(
        Name=enum_name,
        entity_type=EntityType.ENUM,
        namespace=namespace,
    )

    for child in node.get_children():
        if child.kind == clang.cindex.CursorKind.ENUM_CONSTANT_DECL:
            field_name = child.spelling
            field = Field(name=field_name, is_enum=True, original_type="int")
            enum_entity.Fields.append(field)

    return enum_entity


def parse_file(file_path: str) -> List[Entity]:
    entities: List[Entity] = []
    current_entity: Optional[Entity] = None

    file_path = Path(file_path)
    if not file_path.exists():
        raise FileNotFoundError(f"File not found: {file_path}")

    with open(file_path, "r") as f:
        original_content = f.read()

    combined_content = combine_header_with_file(original_content)
    with tempfile.NamedTemporaryFile(mode="w", suffix=".c", delete=False) as f:
        f.write(combined_content)
    temp_filename = f.name
    print(f"Temporary file created at: {temp_filename}")

    try:
        import clang.cindex

        index = clang.cindex.Index.create()
        translation_unit = index.parse(
            temp_filename,
            args=COMPILER_ARGS,
            options=clang.cindex.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD,
        )
    except Exception as e:
        raise RuntimeError(f"Error parsing file {file_path} with clang: {e}")

    target_stem = Path(temp_filename).stem
    data = {"entities": [], "enums": []}

    for node in translation_unit.cursor.walk_preorder():
        if node.location.file and Path(node.location.file.name) != Path(temp_filename):
            continue

        if node.kind == clang.cindex.CursorKind.NAMESPACE:
            namespace = node.spelling
        elif node.kind in (
            clang.cindex.CursorKind.STRUCT_DECL,
            clang.cindex.CursorKind.CLASS_DECL,
        ):
            entity = process_entity(node, None, target_stem)
            if entity:
                data["entities"].append(entity)
        elif node.kind == clang.cindex.CursorKind.ENUM_DECL:
            enum = process_enum(node, None)
            if enum:
                data["enums"].append(enum)
    return data["entities"] + data["enums"]


if __name__ == "__main__":
    cpp_file_path = "../../src/exper/entities.h"
    entities = parse_file(cpp_file_path)
    print_entities(entities, show_fields=True)
