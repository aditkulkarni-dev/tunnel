import os
import re

def extract_class_name(effect_header_path):
    """
    Parses the custom effect header to find the class name.
    Looks for the pattern 'class ClassName' inside the file.
    """
    if not os.path.exists(effect_header_path):
        raise FileNotFoundError(f"Source effect header not found at: {effect_header_path}")
        
    with open(effect_header_path, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()
        
    # Regex to find 'class NAME'
    match = re.search(r'class\s+([A-Za-z0-9_]+)', content)
    if match:
        return match.group(1)
    else:
        raise ValueError("Could not find a valid C++ class definition in the header.")

def include_user_defined_libraries(file_path: str, visited=None, system_includes = None):
    if visited is None:
        visited = set()
    if system_includes is None:
        system_includes = set()

    
    # Use absolute paths to prevent circular dependency resolution issues
    file_path = os.path.abspath(os.path.normpath(file_path))

    if file_path in visited:
        return "", system_includes

    visited.add(file_path)

    # Added explicit UTF-8 encoding to prevent silent truncation on Windows
    with open(file_path, "r", encoding='utf-8', errors='ignore') as f:
        content = f.read()

    output = []

    for line in content.splitlines():
        # Added ^\s* to catch includes that might be indented
        match = re.match(r'^\s*#include\s+"([^"]+)"', line)
        system_match = re.match(r'^\s*#include\s+<([^>]+)>',line)
        if line.strip() == "#pragma once":
            continue

        if match:
            current_dir = os.path.dirname(file_path)
            included_path = os.path.normpath(os.path.join(current_dir, match.group(1)))
            expanded, system_includes = include_user_defined_libraries(included_path, visited, system_includes)

            if expanded:
                output.append(f"\n// ===== BEGIN {included_path} =====\n")
                output.append(expanded)
                output.append(f"\n// ===== END {included_path} =====\n")

        elif system_match:
            system_includes.add(line.strip())

        else:
            output.append(line)

    cpp_path = os.path.splitext(file_path)[0] + ".cpp"

    if file_path.endswith(".h") and os.path.exists(cpp_path):
        expanded_cpp, system_includes = include_user_defined_libraries(
            cpp_path,
            visited,
            system_includes
        )

        if expanded_cpp:
            output.append(f"\n// ===== BEGIN {cpp_path} =====\n")
            output.append(expanded_cpp)
            output.append(f"\n// ===== END {cpp_path} =====\n")

    return "\n".join(output), system_includes

def generate_juce_project(effect_header_path, template_dir, placeholder="NewProject"):
    try:
        effect_class_name = extract_class_name(effect_header_path)
        print(f"Detected Effect Class Name: {effect_class_name}")

        parent_dir = os.path.dirname(os.path.abspath(template_dir))

        output_dir = os.path.join(parent_dir, effect_class_name)
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
            print(f"Created directory: {output_dir}")
        else:
            print(f"Directory {output_dir} already exists. Overwriting files...")
        
        template_files = [
                "PluginProcessor.h",
                "PluginProcessor.cpp",
                "PluginEditor.h",
                "PluginEditor.cpp"
            ]
        
        for filename in template_files:
            src_file_path = os.path.join(template_dir, filename)
            
            if not os.path.exists(src_file_path):
                print(f"Warning: Template file {filename} missing from {template_dir}. Skipping.")
                continue
                
            with open(src_file_path, 'r', encoding='utf-8') as f:
                file_content = f.read()
            
            translated_content = file_content.replace(placeholder, effect_class_name)
            translated_content = translated_content.replace(
                            "{{EFFECT_CLASS_NAME}}",
                            effect_class_name
                        )
            
            
            if filename == "PluginProcessor.h":
                # Validate that the placeholder actually exists in the template
                if "{{EFFECT_HEADER}}" not in translated_content:
                    print(f"\n[!] ERROR: Could not find '{{{{EFFECT_HEADER}}}}' exact match in {filename}!")
                else:
                    translated_content = translated_content.replace(
                                        "{{EFFECT_HEADER}}",
                                        f'#include "{SRC_EFFECT_HEADER}"'
                                    )
                    
                    
                    print("Snippet of injection:\n", translated_content[:500], "\n...[truncated]...\n")

            dest_file_path = os.path.join(output_dir, filename)
            
            with open(dest_file_path, 'w', encoding='utf-8') as f:
                f.write(translated_content)
                
            print(f"  -> Generated: {dest_file_path}")
                
        print("\nSuccess! Minimal boilerplate translation complete.")
    except Exception as e:
        print(f"Error occurred: {e}")

if __name__ == "__main__":
    SRC_EFFECT_HEADER = "../effects/FfDelay.h"
    JUCE_TEMPLATE_DIR = "../juce"
    generate_juce_project(SRC_EFFECT_HEADER, JUCE_TEMPLATE_DIR)