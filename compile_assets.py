import subprocess
import os
import base64
import zlib
from pathlib import Path

MAX_PACK_SIZE = 10 * 1024 * 1024

final_data_path = 'build/src/Debug/data'

shader_resources = [];
shader_offset = 0;
shader_resource_file_bytes = b""

texture_resources = {}
texture_offset = 0
texture_resource_bytes = {}
texture_current_pack = 1

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
        size=shader_size,
        pack_file = 1
    ));
    shader_offset = shader_offset + shader_size
    
    shader_resource_file_bytes = shader_resource_file_bytes + shader_bytes;
    
def process_texture_asset(file_path, rel_path):
    directory = Path(file_path).parent
    file_name = Path(file_path).name
    resource_path = rel_path.replace('/', '::');
    texture_bytes = b""
    with open(file_path, 'rb') as file:
        texture_bytes = zlib.compress(file.read(), level=9)
    texture_size = len(texture_bytes)
    
    global texture_resources
    global texture_offset
    global texture_resource_bytes
    global texture_current_pack
    
    if texture_current_pack not in texture_resource_bytes:
        texture_resource_bytes[texture_current_pack] = b""
    if texture_current_pack not in texture_resources:
        texture_resources[texture_current_pack] = []
    
    texture_resources[texture_current_pack].append (dict(
        resource_name=resource_path,
        offset=texture_offset,
        size=texture_size,
        pack_file=texture_current_pack
    ));
    texture_offset = texture_offset + texture_size
    
    texture_resource_bytes[texture_current_pack] = texture_resource_bytes[texture_current_pack] + texture_bytes;

    if texture_offset > MAX_PACK_SIZE:
        texture_current_pack = texture_current_pack + 1
        texture_offset = 0


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

def serialize_shader_resources(file_name, resource_map, data):
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
    
def serialize_texture_resources(resource_map, data_map):
    map_str = str(resource_map);
    map_str = map_str.encode("utf-8")
    base64_bytes = base64.b64encode(map_str)
    base64_string = base64_bytes.decode("ascii")
    with open("t000.dat", "wb") as file:
        file.write(base64_bytes)
        
    global final_data_path
    
    for key, value in data_map.items():
        file_data = value
        file_name = f"t{key:03d}.pack"
        file_path = f"{final_data_path}/{file_name}"
        with open(file_path, "wb") as file:
            file.write(file_data)
        


process_assets('resources/shaders', ['.comp', '.vert', '.frag'], process_shader_asset);
serialize_shader_resources(f'{final_data_path}/s001.pack', shader_resources, shader_resource_file_bytes)

process_assets('resources/textures', ['.png', '.jpg', '.bmp'], process_texture_asset);
serialize_texture_resources(texture_resources, texture_resource_bytes)