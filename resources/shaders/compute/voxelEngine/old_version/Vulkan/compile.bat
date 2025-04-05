glslc.exe shader.comp -o compute.spv

glslc.exe heightmap_field.comp -o voxel/heightmap_field.spv
glslc.exe iso_field.comp -o voxel/iso_field.spv
glslc.exe material_field.comp -o voxel/material_field.spv
glslc.exe unify_fields.comp -o voxel/unify_fields.spv

glslc.exe smoothrender_createmesh.comp -o voxel/smoothrender_createmesh.spv
glslc.exe smoothrender_createvertlist.comp -o voxel/smoothrender_createvertlist.spv

glslc.exe smoothrender_construct.comp -o voxel/smoothrender_construct.spv
glslc.exe smoothrender_mark.comp -o voxel/smoothrender_mark.spv
glslc.exe smoothrender_stitch.comp -o voxel/smoothrender_stitch.spv
glslc.exe smoothrender_stitch_async.comp -o voxel/smoothrender_stitch_async.spv

pause