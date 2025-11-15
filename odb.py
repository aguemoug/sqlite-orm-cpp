#!/usr/bin/env python3
"""
CPP-SQLite-ORM: A mini ORM for SQLite and C++
Generates CRUD operations from C++ struct definitions with annotations.
"""
import argparse
import os
import subprocess
import sys
import tempfile
from datetime import datetime
from parser import Entity, EntityType, Field, parse_file
from pathlib import Path

from jinja2 import Template


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


def generate_header(
    source_file: str,
    template: str,
    namespace: str = "data",
    output_file: str = "output.hpp",
):
    """Generate complete CRUD header file with operations for both tables and views"""

    entities = parse_file(source_file)
    # Separate entities by type
    tables = [
        e for e in entities if not e.is_ignored and e.entity_type == EntityType.TABLE
    ]
    views = [
        e for e in entities if not e.is_ignored and e.entity_type == EntityType.VIEW
    ]
    enums = [
        e for e in entities if not e.is_ignored and e.entity_type == EntityType.ENUM
    ]
    all_entities = tables + views

    # Load master template
    try:
        template_path = Path(template)
        if not template_path.exists():
            raise FileNotFoundError(f"Template file not found: {template_path}")
        tmpl = Template(open(template).read())

        header_content = tmpl.render(
            timestamp=datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            source_file=os.path.basename(source_file),
            namespace=namespace,
            entities=all_entities,
            enums=enums,
            tables=tables,
            views=views,
        )

        header_content = format_with_clangformat(header_content)

        with open(output_file, "w") as f:
            f.write(header_content)

        print(f"Successfully generated: {output_file}")
        print(f"Tables processed: {len(tables)} :{', '.join([t.Name for t in tables])}")
        print(f"Views processed: {len(views)} :{', '.join([v.Name for v in views])}")
        print(f"Enums processed: {len(enums)} :{', '.join([e.Name for e in enums])}")
    except Exception as e:
        print(f"Generation error : {e}")
        sys.exit(1)


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
        "input_file",
        default="entities.h",
        help="Input C++ header file with struct definitions",
    )

    parser.add_argument(
        "-o",
        "--output",
        default="output.hpp",
        help="Output header file (default: output.hpp)",
    )

    parser.add_argument(
        "-t",
        "--template",
        default="./",
        help="templates directory (default: templates)",
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
        "-v",
        "--verbose",
        default=True,
        action="store_true",
        help="Enable verbose output",
    )

    parser.add_argument("--version", action="version", version="CPP-SQLite-ORM 1.0.0")

    args = parser.parse_args()

    # Check if input file exists
    if not os.path.exists(args.input_file):
        print(f"Error: Input file '{args.input_file}' not found")
        sys.exit(1)

    # Check if templates directory exists
    if not os.path.exists(args.template):
        print("Error: 'templates' directory not found")
        print("Please ensure you're running from the project root directory")
        sys.exit(1)

    try:
        generate_header(
            source_file=args.input_file,
            template=os.path.join(args.template, "soci.jinja"),
            namespace=args.namespace,
            output_file=args.output,
        )

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
