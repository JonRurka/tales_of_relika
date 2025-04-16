import subprocess
import os
import base64
import zlib
from pathlib import Path

shader_resources = [];
shader_offset = 0;
shader_resource_file_bytes = b""

def compile_shader(working_dir, resource_path, file):
    if file != 'standard.frag':
        return "";
    
    result = subprocess.run(
        ['glslc.exe', file, '-o', '-', '--target-env=opengl4.5'], 
        capture_output=True,  
        cwd=working_dir
    )

    if result.returncode == 0:
        print(f"Shader '{resource_path}' compiled successfully.")
        output_bytes = result.stdout
        compressed_data = zlib.compress(output_bytes, level=9)
        return compressed_data;
    else:
        print(f"Command failed with return code: {result.returncode}")
        #print(f"Error: {result.stderr}")
        output_str = str(result.stderr)
        if 'error generated.' in output_str:
            output_lines = output_str.split('\\n')
            print(f'Errors Generated for {file}:');
            for x in range(len(output_lines)):
                print (output_lines[x].replace('\\r', '').replace('\\n', ''))
        return "";


def process_shader_asset(file_path, rel_path):
    directory = Path(file_path).parent
    file_name = Path(file_path).name
    resource_path = rel_path.replace('/', '::');
    shader_bytes = compile_shader(directory, resource_path, file_name)
    shader_size = len(shader_bytes)
    #print(len(shader_bytes))
    
    if shader_bytes == "":
        return
    
    global shader_resources;
    global shader_offset;
    global shader_resource_file_bytes;
    
    shader_resources.append(dict(
        resource_name=resource_path,
        offset=shader_offset,
        size=shader_size
    ));
    shader_offset = shader_offset + shader_size
    
    shader_resource_file_bytes = shader_resource_file_bytes + shader_bytes;
    

def process_assets(directory, extension_list, func):
    # Walk through directory and its subdirectories
    for root, dirs, files in os.walk(directory):        
        # Print files
        for file_name in files:
            file_path = os.path.join(root, file_name).replace('\\', '/');
            rel_path = file_path.replace(directory, '');
            if rel_path.startswith('/'):
                rel_path = rel_path[1:]
            extension = Path(rel_path).suffix;
            if (extension.lower() in extension_list):
                func(file_path, rel_path);
            

def serialize_resources(file_name, resource_map, data):
    map_str = str(resource_map);
    map_str = map_str.encode("utf-8")
    base64_bytes = base64.b64encode(map_str)
    base64_string = base64_bytes.decode("ascii")
    resource_offset = len(data)
    resource_offset_bytes = resource_offset.to_bytes(4, byteorder="little")
    
    file_data = resource_offset_bytes + data + base64_bytes
    
    directory = Path(file_name).parent
    os.makedirs(directory, exist_ok=True)
    with open(file_name, "wb") as file:
        file.write(file_data)
    
    
    


process_assets('C:/Users/jrurka/Source/repos/game_project/resources/shaders', ['.comp', '.vert', '.frag'], process_shader_asset);
serialize_resources('build/src/Debug/data/s001.pack', shader_resources, shader_resource_file_bytes)


