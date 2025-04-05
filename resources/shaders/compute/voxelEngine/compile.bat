
glslc.exe 1-heightmap_field_gen.comp -o Bin/1-heightmap_field_gen.spv
glslc.exe 2-iso_field_gen.comp -o Bin/2-iso_field_gen.spv
glslc.exe 3-unify_fields.comp -o Bin/3-unify_fields.spv

glslc.exe 4-smoothrender_construct.comp -o Bin/4-smoothrender_construct.spv
glslc.exe 5-smoothrender_mark.comp -o Bin/5-smoothrender_mark.spv
glslc.exe 6-smoothrender_async_stitch.comp -o Bin/6-smoothrender_async_stitch.spv

pause