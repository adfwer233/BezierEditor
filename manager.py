import json
import argparse
import os

project_json_name = "GraphicsLabProject.json"

def update_project_json(project_path: str, output_dll: str, build_type: str):

    project_json_path = os.path.join(project_path, project_json_name)
    project_name = os.path.basename(project_path)
    try:
        with open(project_json_path, "r") as f:
            data = json.load(f)
    except FileNotFoundError:
        data = {"Project": "", "Built": []}

    # Update project name and DLL path for the build type
    data["Project"] = project_name

    new_built = []

    new_built.append ({
        "build_type": build_type,
        "dll_path": output_dll
    })

    for built in data["Built"]:
        if built["build_type"] != build_type:
            new_built.append(built)

    data["Built"] = new_built

    # Write the updated data back to the JSON file
    with open(project_json_path, "w") as f:
        json.dump(data, f, indent=4)

if __name__ == "__main__":
    print("Script Running")
    # Set up argument parsing
    parser = argparse.ArgumentParser(description="Update project JSON with DLL path and build type.")
    parser.add_argument("--project-dir", required=True, help="Path to the project directory.")
    parser.add_argument("--output-dll", required=True, help="Path to the output DLL.")
    parser.add_argument("--build-type", required=True, help="Build type (e.g., Debug, Release).")

    # Parse the arguments
    args = parser.parse_args()

    # Call the update function with the parsed arguments
    update_project_json(args.project_dir, args.output_dll, args.build_type)
