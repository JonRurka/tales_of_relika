# 0 "5-smoothrender_mark.cl"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "5-smoothrender_mark.cl"



# 1 "../libUSL/USL.inc" 1



# 1 "../libUSL/Types.inc" 1
# 5 "../libUSL/USL.inc" 2
# 1 "../libUSL/Buffers.inc" 1
# 6 "../libUSL/USL.inc" 2
# 1 "../libUSL/compat.inc" 1
# 16 "../libUSL/compat.inc"
float sqrMagnitude(float3 val) {

 return dot(val, val);
}
# 33 "../libUSL/compat.inc"
float vector_angle(float3 from, float3 to){
    float num = sqrt(dot(from, from) * dot(to, to));
    if (num < 0.0000000000000001)
    {
        return 0;
    }
 float dot_val2 = dot(from, to);
    float num2 = clamp(dot_val2 / num, -1.0f, 1.0f);
    return acos(num2) * 57.29578f;
}
# 7 "../libUSL/USL.inc" 2
# 5 "5-smoothrender_mark.cl" 2
# 1 "Voxel_Lib/smoothrender_mark_lib.inc" 1



# 1 "Voxel_Lib/voxel_lib_core.inc" 1



# 1 "Voxel_Lib/../../libUSL/USL.inc" 1
# 5 "Voxel_Lib/voxel_lib_core.inc" 2
# 1 "Voxel_Lib/../../libnoise/libnoise.inc" 1



# 1 "Voxel_Lib/../../libnoise/libnoise_core.inc" 1



# 1 "Voxel_Lib/../../libnoise/../libUSL/USL.inc" 1
# 5 "Voxel_Lib/../../libnoise/libnoise_core.inc" 2
# 1 "Voxel_Lib/../../libnoise/libnoise_const.inc" 1
# 6 "Voxel_Lib/../../libnoise/libnoise_core.inc" 2
# 1 "Voxel_Lib/../../libnoise/libnoise_objects.inc" 1




constant float g_randomVectors[1024] = {
    -0.763874, -0.596439, -0.246489, 0.0,
    0.396055, 0.904518, -0.158073, 0.0,
    -0.499004, -0.8665, -0.0131631, 0.0,
    0.468724, -0.824756, 0.316346, 0.0,
    0.829598, 0.43195, 0.353816, 0.0,
    -0.454473, 0.629497, -0.630228, 0.0,
    -0.162349, -0.869962, -0.465628, 0.0,
    0.932805, 0.253451, 0.256198, 0.0,
    -0.345419, 0.927299, -0.144227, 0.0,
    -0.715026, -0.293698, -0.634413, 0.0,
    -0.245997, 0.717467, -0.651711, 0.0,
    -0.967409, -0.250435, -0.037451, 0.0,
    0.901729, 0.397108, -0.170852, 0.0,
    0.892657, -0.0720622, -0.444938, 0.0,
    0.0260084, -0.0361701, 0.999007, 0.0,
    0.949107, -0.19486, 0.247439, 0.0,
    0.471803, -0.807064, -0.355036, 0.0,
    0.879737, 0.141845, 0.453809, 0.0,
    0.570747, 0.696415, 0.435033, 0.0,
    -0.141751, -0.988233, -0.0574584, 0.0,
    -0.58219, -0.0303005, 0.812488, 0.0,
    -0.60922, 0.239482, -0.755975, 0.0,
    0.299394, -0.197066, -0.933557, 0.0,
    -0.851615, -0.220702, -0.47544, 0.0,
    0.848886, 0.341829, -0.403169, 0.0,
    -0.156129, -0.687241, 0.709453, 0.0,
    -0.665651, 0.626724, 0.405124, 0.0,
    0.595914, -0.674582, 0.43569, 0.0,
    0.171025, -0.509292, 0.843428, 0.0,
    0.78605, 0.536414, -0.307222, 0.0,
    0.18905, -0.791613, 0.581042, 0.0,
    -0.294916, 0.844994, 0.446105, 0.0,
    0.342031, -0.58736, -0.7335, 0.0,
    0.57155, 0.7869, 0.232635, 0.0,
    0.885026, -0.408223, 0.223791, 0.0,
    -0.789518, 0.571645, 0.223347, 0.0,
    0.774571, 0.31566, 0.548087, 0.0,
    -0.79695, -0.0433603, -0.602487, 0.0,
    -0.142425, -0.473249, -0.869339, 0.0,
    -0.0698838, 0.170442, 0.982886, 0.0,
    0.687815, -0.484748, 0.540306, 0.0,
    0.543703, -0.534446, -0.647112, 0.0,
    0.97186, 0.184391, -0.146588, 0.0,
    0.707084, 0.485713, -0.513921, 0.0,
    0.942302, 0.331945, 0.043348, 0.0,
    0.499084, 0.599922, 0.625307, 0.0,
    -0.289203, 0.211107, 0.9337, 0.0,
    0.412433, -0.71667, -0.56239, 0.0,
    0.87721, -0.082816, 0.47291, 0.0,
    -0.420685, -0.214278, 0.881538, 0.0,
    0.752558, -0.0391579, 0.657361, 0.0,
    0.0765725, -0.996789, 0.0234082, 0.0,
    -0.544312, -0.309435, -0.779727, 0.0,
    -0.455358, -0.415572, 0.787368, 0.0,
    -0.874586, 0.483746, 0.0330131, 0.0,
    0.245172, -0.0838623, 0.965846, 0.0,
    0.382293, -0.432813, 0.81641, 0.0,
    -0.287735, -0.905514, 0.311853, 0.0,
    -0.667704, 0.704955, -0.239186, 0.0,
    0.717885, -0.464002, -0.518983, 0.0,
    0.976342, -0.214895, 0.0240053, 0.0,
    -0.0733096, -0.921136, 0.382276, 0.0,
    -0.986284, 0.151224, -0.0661379, 0.0,
    -0.899319, -0.429671, 0.0812908, 0.0,
    0.652102, -0.724625, 0.222893, 0.0,
    0.203761, 0.458023, -0.865272, 0.0,
    -0.030396, 0.698724, -0.714745, 0.0,
    -0.460232, 0.839138, 0.289887, 0.0,
    -0.0898602, 0.837894, 0.538386, 0.0,
    -0.731595, 0.0793784, 0.677102, 0.0,
    -0.447236, -0.788397, 0.422386, 0.0,
    0.186481, 0.645855, -0.740335, 0.0,
    -0.259006, 0.935463, 0.240467, 0.0,
    0.445839, 0.819655, -0.359712, 0.0,
    0.349962, 0.755022, -0.554499, 0.0,
    -0.997078, -0.0359577, 0.0673977, 0.0,
    -0.431163, -0.147516, -0.890133, 0.0,
    0.299648, -0.63914, 0.708316, 0.0,
    0.397043, 0.566526, -0.722084, 0.0,
    -0.502489, 0.438308, -0.745246, 0.0,
    0.0687235, 0.354097, 0.93268, 0.0,
    -0.0476651, -0.462597, 0.885286, 0.0,
    -0.221934, 0.900739, -0.373383, 0.0,
    -0.956107, -0.225676, 0.186893, 0.0,
    -0.187627, 0.391487, -0.900852, 0.0,
    -0.224209, -0.315405, 0.92209, 0.0,
    -0.730807, -0.537068, 0.421283, 0.0,
    -0.0353135, -0.816748, 0.575913, 0.0,
    -0.941391, 0.176991, -0.287153, 0.0,
    -0.154174, 0.390458, 0.90762, 0.0,
    -0.283847, 0.533842, 0.796519, 0.0,
    -0.482737, -0.850448, 0.209052, 0.0,
    -0.649175, 0.477748, 0.591886, 0.0,
    0.885373, -0.405387, -0.227543, 0.0,
    -0.147261, 0.181623, -0.972279, 0.0,
    0.0959236, -0.115847, -0.988624, 0.0,
    -0.89724, -0.191348, 0.397928, 0.0,
    0.903553, -0.428461, -0.00350461, 0.0,
    0.849072, -0.295807, -0.437693, 0.0,
    0.65551, 0.741754, -0.141804, 0.0,
    0.61598, -0.178669, 0.767232, 0.0,
    0.0112967, 0.932256, -0.361623, 0.0,
    -0.793031, 0.258012, 0.551845, 0.0,
    0.421933, 0.454311, 0.784585, 0.0,
    -0.319993, 0.0401618, -0.946568, 0.0,
    -0.81571, 0.551307, -0.175151, 0.0,
    -0.377644, 0.00322313, 0.925945, 0.0,
    0.129759, -0.666581, -0.734052, 0.0,
    0.601901, -0.654237, -0.457919, 0.0,
    -0.927463, -0.0343576, -0.372334, 0.0,
    -0.438663, -0.868301, -0.231578, 0.0,
    -0.648845, -0.749138, -0.133387, 0.0,
    0.507393, -0.588294, 0.629653, 0.0,
    0.726958, 0.623665, 0.287358, 0.0,
    0.411159, 0.367614, -0.834151, 0.0,
    0.806333, 0.585117, -0.0864016, 0.0,
    0.263935, -0.880876, 0.392932, 0.0,
    0.421546, -0.201336, 0.884174, 0.0,
    -0.683198, -0.569557, -0.456996, 0.0,
    -0.117116, -0.0406654, -0.992285, 0.0,
    -0.643679, -0.109196, -0.757465, 0.0,
    -0.561559, -0.62989, 0.536554, 0.0,
    0.0628422, 0.104677, -0.992519, 0.0,
    0.480759, -0.2867, -0.828658, 0.0,
    -0.228559, -0.228965, -0.946222, 0.0,
    -0.10194, -0.65706, -0.746914, 0.0,
    0.0689193, -0.678236, 0.731605, 0.0,
    0.401019, -0.754026, 0.52022, 0.0,
    -0.742141, 0.547083, -0.387203, 0.0,
    -0.00210603, -0.796417, -0.604745, 0.0,
    0.296725, -0.409909, -0.862513, 0.0,
    -0.260932, -0.798201, 0.542945, 0.0,
    -0.641628, 0.742379, 0.192838, 0.0,
    -0.186009, -0.101514, 0.97729, 0.0,
    0.106711, -0.962067, 0.251079, 0.0,
    -0.743499, 0.30988, -0.592607, 0.0,
    -0.795853, -0.605066, -0.0226607, 0.0,
    -0.828661, -0.419471, -0.370628, 0.0,
    0.0847218, -0.489815, -0.8677, 0.0,
    -0.381405, 0.788019, -0.483276, 0.0,
    0.282042, -0.953394, 0.107205, 0.0,
    0.530774, 0.847413, 0.0130696, 0.0,
    0.0515397, 0.922524, 0.382484, 0.0,
    -0.631467, -0.709046, 0.313852, 0.0,
    0.688248, 0.517273, 0.508668, 0.0,
    0.646689, -0.333782, -0.685845, 0.0,
    -0.932528, -0.247532, -0.262906, 0.0,
    0.630609, 0.68757, -0.359973, 0.0,
    0.577805, -0.394189, 0.714673, 0.0,
    -0.887833, -0.437301, -0.14325, 0.0,
    0.690982, 0.174003, 0.701617, 0.0,
    -0.866701, 0.0118182, 0.498689, 0.0,
    -0.482876, 0.727143, 0.487949, 0.0,
    -0.577567, 0.682593, -0.447752, 0.0,
    0.373768, 0.0982991, 0.922299, 0.0,
    0.170744, 0.964243, -0.202687, 0.0,
    0.993654, -0.035791, -0.106632, 0.0,
    0.587065, 0.4143, -0.695493, 0.0,
    -0.396509, 0.26509, -0.878924, 0.0,
    -0.0866853, 0.83553, -0.542563, 0.0,
    0.923193, 0.133398, -0.360443, 0.0,
    0.00379108, -0.258618, 0.965972, 0.0,
    0.239144, 0.245154, -0.939526, 0.0,
    0.758731, -0.555871, 0.33961, 0.0,
    0.295355, 0.309513, 0.903862, 0.0,
    0.0531222, -0.91003, -0.411124, 0.0,
    0.270452, 0.0229439, -0.96246, 0.0,
    0.563634, 0.0324352, 0.825387, 0.0,
    0.156326, 0.147392, 0.976646, 0.0,
    -0.0410141, 0.981824, 0.185309, 0.0,
    -0.385562, -0.576343, -0.720535, 0.0,
    0.388281, 0.904441, 0.176702, 0.0,
    0.945561, -0.192859, -0.262146, 0.0,
    0.844504, 0.520193, 0.127325, 0.0,
    0.0330893, 0.999121, -0.0257505, 0.0,
    -0.592616, -0.482475, -0.644999, 0.0,
    0.539471, 0.631024, -0.557476, 0.0,
    0.655851, -0.027319, -0.754396, 0.0,
    0.274465, 0.887659, 0.369772, 0.0,
    -0.123419, 0.975177, -0.183842, 0.0,
    -0.223429, 0.708045, 0.66989, 0.0,
    -0.908654, 0.196302, 0.368528, 0.0,
    -0.95759, -0.00863708, 0.288005, 0.0,
    0.960535, 0.030592, 0.276472, 0.0,
    -0.413146, 0.907537, 0.0754161, 0.0,
    -0.847992, 0.350849, -0.397259, 0.0,
    0.614736, 0.395841, 0.68221, 0.0,
    -0.503504, -0.666128, -0.550234, 0.0,
    -0.268833, -0.738524, -0.618314, 0.0,
    0.792737, -0.60001, -0.107502, 0.0,
    -0.637582, 0.508144, -0.579032, 0.0,
    0.750105, 0.282165, -0.598101, 0.0,
    -0.351199, -0.392294, -0.850155, 0.0,
    0.250126, -0.960993, -0.118025, 0.0,
    -0.732341, 0.680909, -0.0063274, 0.0,
    -0.760674, -0.141009, 0.633634, 0.0,
    0.222823, -0.304012, 0.926243, 0.0,
    0.209178, 0.505671, 0.836984, 0.0,
    0.757914, -0.56629, -0.323857, 0.0,
    -0.782926, -0.339196, 0.52151, 0.0,
    -0.462952, 0.585565, 0.665424, 0.0,
    0.61879, 0.194119, -0.761194, 0.0,
    0.741388, -0.276743, 0.611357, 0.0,
    0.707571, 0.702621, 0.0752872, 0.0,
    0.156562, 0.819977, 0.550569, 0.0,
    -0.793606, 0.440216, 0.42, 0.0,
    0.234547, 0.885309, -0.401517, 0.0,
    0.132598, 0.80115, -0.58359, 0.0,
    -0.377899, -0.639179, 0.669808, 0.0,
    -0.865993, -0.396465, 0.304748, 0.0,
    -0.624815, -0.44283, 0.643046, 0.0,
    -0.485705, 0.825614, -0.287146, 0.0,
    -0.971788, 0.175535, 0.157529, 0.0,
    -0.456027, 0.392629, 0.798675, 0.0,
    -0.0104443, 0.521623, -0.853112, 0.0,
    -0.660575, -0.74519, 0.091282, 0.0,
    -0.0157698, -0.307475, -0.951425, 0.0,
    -0.603467, -0.250192, 0.757121, 0.0,
    0.506876, 0.25006, 0.824952, 0.0,
    0.255404, 0.966794, 0.00884498, 0.0,
    0.466764, -0.874228, -0.133625, 0.0,
    0.475077, -0.0682351, -0.877295, 0.0,
    -0.224967, -0.938972, -0.260233, 0.0,
    -0.377929, -0.814757, -0.439705, 0.0,
    -0.305847, 0.542333, -0.782517, 0.0,
    0.26658, -0.902905, -0.337191, 0.0,
    0.0275773, 0.322158, -0.946284, 0.0,
    0.0185422, 0.716349, 0.697496, 0.0,
    -0.20483, 0.978416, 0.0273371, 0.0,
    -0.898276, 0.373969, 0.230752, 0.0,
    -0.00909378, 0.546594, 0.837349, 0.0,
    0.6602, -0.751089, 0.000959236, 0.0,
    0.855301, -0.303056, 0.420259, 0.0,
    0.797138, 0.0623013, -0.600574, 0.0,
    0.48947, -0.866813, 0.0951509, 0.0,
    0.251142, 0.674531, 0.694216, 0.0,
    -0.578422, -0.737373, -0.348867, 0.0,
    -0.254689, -0.514807, 0.818601, 0.0,
    0.374972, 0.761612, 0.528529, 0.0,
    0.640303, -0.734271, -0.225517, 0.0,
    -0.638076, 0.285527, 0.715075, 0.0,
    0.772956, -0.15984, -0.613995, 0.0,
    0.798217, -0.590628, 0.118356, 0.0,
    -0.986276, -0.0578337, -0.154644, 0.0,
    -0.312988, -0.94549, 0.0899272, 0.0,
    -0.497338, 0.178325, 0.849032, 0.0,
    -0.101136, -0.981014, 0.165477, 0.0,
    -0.521688, 0.0553434, -0.851339, 0.0,
    -0.786182, -0.583814, 0.202678, 0.0,
    -0.565191, 0.821858, -0.0714658, 0.0,
    0.437895, 0.152598, -0.885981, 0.0,
    -0.92394, 0.353436, -0.14635, 0.0,
    0.212189, -0.815162, -0.538969, 0.0,
    -0.859262, 0.143405, -0.491024, 0.0,
    0.991353, 0.112814, 0.0670273, 0.0,
    0.0337884, -0.979891, -0.196654, 0.0
};

struct Module{
   float Value;
   float m_X;
   float m_Y;
   float m_Z;

   float m_frequency;
   float m_lacunarity;
   int m_noiseQuality;
   float m_octaveCount;
   float m_persistence;
   int m_seed;


   float m_ridged_offset;
   float m_ridged_gain;
   float m_ridged_H;


   float m_voronoi_displacement;
   bool m_voronoi_enableDistance;


   float m_cachedValue;
   bool m_cache_isCached;
   float m_cache_xCache;
   float m_cache_yCache;
   float m_cache_zCache;


   float m_x1Matrix;
   float m_x2Matrix;
   float m_x3Matrix;
   float m_xAngle;
   float m_y1Matrix;
   float m_y2Matrix;
   float m_y3Matrix;
   float m_yAngle;
   float m_z1Matrix;
   float m_z2Matrix;
   float m_z3Matrix;
   float m_zAngle;


   float m_edgeFalloff;
   float m_lowerBound;
   float m_upperBound;

   float m_turb_power;
   float m_turb_seed;
   float m_turb_frequency;
   float m_turb_roughness;

};

struct ControlPoint {

   float inputValue;

   float outputValue;

};
# 7 "Voxel_Lib/../../libnoise/libnoise_core.inc" 2
# 16 "Voxel_Lib/../../libnoise/libnoise_core.inc"
struct Module GetModule_1(struct Module m)
{
   struct Module module;

   module.Value = m.Value;
   module.m_X = m.m_X;
   module.m_Y = m.m_Y;
   module.m_Z = m.m_Z;


   module.m_frequency = m.m_frequency;
   module.m_lacunarity = m.m_lacunarity;
   module.m_noiseQuality = m.m_noiseQuality;
   module.m_octaveCount = m.m_octaveCount;
   module.m_persistence = m.m_persistence;
   module.m_seed = m.m_seed;


   module.m_ridged_offset = m.m_ridged_offset;
   module.m_ridged_gain = m.m_ridged_gain;
   module.m_ridged_H = m.m_ridged_H;


   module.m_voronoi_displacement = m.m_voronoi_displacement;
   module.m_voronoi_enableDistance = m.m_voronoi_enableDistance;


   module.m_cachedValue = m.m_cachedValue;
   module.m_cache_isCached = m.m_cache_isCached;
   module.m_cache_xCache = m.m_cache_xCache;
   module.m_cache_yCache = m.m_cache_yCache;
   module.m_cache_zCache = m.m_cache_zCache;


   module.m_x1Matrix = m.m_x1Matrix;
   module.m_x2Matrix = m.m_x2Matrix;
   module.m_x3Matrix = m.m_x3Matrix;
   module.m_xAngle = m.m_xAngle;
   module.m_y1Matrix = m.m_y1Matrix;
   module.m_y2Matrix = m.m_y2Matrix;
   module.m_y3Matrix = m.m_y3Matrix;
   module.m_yAngle = m.m_yAngle;
   module.m_z1Matrix = m.m_z1Matrix;
   module.m_z2Matrix = m.m_z2Matrix;
   module.m_z3Matrix = m.m_z3Matrix;
   module.m_zAngle = m.m_zAngle;


   module.m_edgeFalloff = m.m_edgeFalloff;
   module.m_lowerBound = m.m_lowerBound;
   module.m_upperBound = m.m_upperBound;


   module.m_turb_power = m.m_turb_power;
   module.m_turb_seed = m.m_turb_seed;
   module.m_turb_frequency = m.m_turb_frequency;
   module.m_turb_roughness = m.m_turb_roughness;

   return module;
}

struct Module GetModule_2(struct Module m, float value)
{
   struct Module module = GetModule_1(m);
   module.Value = value;
   return module;
}

struct Module GetModule_3(float x, float y, float z)
{
   struct Module module;

   module.Value = 0;
   module.m_X = x;
   module.m_Y = y;
   module.m_Z = z;


   module.m_frequency = 1.0;
   module.m_lacunarity = 2.0;
   module.m_noiseQuality = 1;
   module.m_octaveCount = 6;
   module.m_persistence = 0.5;
   module.m_seed = 0;


   module.m_ridged_offset = 1.0;
   module.m_ridged_gain = 2.0;
   module.m_ridged_H = 1.0;


   module.m_voronoi_displacement = 1.0;
   module.m_voronoi_enableDistance = false;


   module.m_cachedValue = 0;
   module.m_cache_isCached = false;
   module.m_cache_xCache = 0;
   module.m_cache_yCache = 0;
   module.m_cache_zCache = 0;


   module.m_turb_power = 1.0;
   module.m_turb_seed = 0;
   module.m_turb_frequency = 1.0;
   module.m_turb_roughness = 3;

   return module;
}

struct Module GetModule_4(float x, float y, float z, float value)
{
   struct Module module = GetModule_3(x, y, z);
   module.Value = value;
   return module;
}

float3 LatLonToXYZ(float lat, float lon)
{
   float r = cos(0.01745329 * lat);
   float x = r * cos(0.01745329 * lon);
   float y = sin(0.01745329 * lat);
   float z = r * sin(0.01745329 * lon);

   return (float3)(x, y, z);
}

int ClampValue(int value, int lowerBound, int upperBound)
{
   return clamp(value, lowerBound, upperBound);
# 155 "Voxel_Lib/../../libnoise/libnoise_core.inc"
}

float CubicInterp(float n0, float n1, float n2, float n3, float a)
{
   float p = (n3 - n2) - (n0 - n1);
   float q = (n0 - n1) - p;
   float r = n2 - n0;
   float s = n1;
   return p * a * a * a + q * a * a + r * a + s;
}

float LinearInterp(float n0, float n1, float a)
{
   return ((1.0 - a) * n0) + (a * n1);
}

float SCurve3(float a)
{
   return (a * a * (3.0 - 2.0 * a));
}

float SCurve5(float a)
{
   float a3 = a * a * a;
   float a4 = a3 * a;
   float a5 = a4 * a;
   return (6.0 * a5) - (15.0 * a4) + (10.0 * a3);
}

float MakeInt32Range(float n)
{
   if (n >= 1073741824.0) {
      return (2.0 * (n - (float)(1073741824.0) * floor(n/(float)(1073741824.0)))) - 1073741824.0;
   }
   else if (n <= -1073741824.0) {
      return (2.0 * (n - (float)(1073741824.0) * floor(n/(float)(1073741824.0)))) + 1073741824.0;
   }
   else {
      return n;
   }
}

int IntValueNoise3D(int x, int y, int z, int seed)
{


   int n = (
      1619 * x
      + 31337 * y
      + 6971 * z
      + 1013 * seed)
      & 0x7fffffff;
   n = (n >> 13) ^ n;
   return (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
}

float ValueNoise3D(int x, int y, int z, int seed)
{
   return 1.0 - ((float)(IntValueNoise3D(x, y, z, seed)) / 1073741824.0);
}

float GradientNoise3D(float fx, float fy, float fz, int ix, int iy, int iz, int seed)
{



   int vectorIndex = (
        1619 * ix
      + 31337 * iy
      + 6971 * iz
      + 1013 * seed)
      & 0xffffffff;
   vectorIndex ^= (vectorIndex >> 8);
   vectorIndex &= 0xff;

   float xvGradient = g_randomVectors[(vectorIndex << 2)];
   float yvGradient = g_randomVectors[(vectorIndex << 2) + 1];
   float zvGradient = g_randomVectors[(vectorIndex << 2) + 2];



   float xvPoint = (fx - (float)(ix));
   float yvPoint = (fy - (float)(iy));
   float zvPoint = (fz - (float)(iz));




   return ((xvGradient * xvPoint)
      + (yvGradient * yvPoint)
      + (zvGradient * zvPoint)) * 2.12;
}

float GradientCoherentNoise3D(float x, float y, float z, int seed, int noiseQuality)
{


   int x0 = (x > 0.0 ? (int)(x) : (int)(x) - 1);
   int x1 = x0 + 1;
   int y0 = (y > 0.0 ? (int)(y) : (int)(y) - 1);
   int y1 = y0 + 1;
   int z0 = (z > 0.0 ? (int)(z) : (int)(z) - 1);
   int z1 = z0 + 1;



   float xs = 0, ys = 0, zs = 0;
   switch (noiseQuality) {
   case 0:
      xs = (x - (float)(x0));
      ys = (y - (float)(y0));
      zs = (z - (float)(z0));
      break;
   case 1:
      xs = SCurve3(x - (float)(x0));
      ys = SCurve3(y - (float)(y0));
      zs = SCurve3(z - (float)(z0));
      break;
   case 2:
      xs = SCurve5(x - (float)(x0));
      ys = SCurve5(y - (float)(y0));
      zs = SCurve5(z - (float)(z0));
      break;
   }





   float n0, n1, ix0, ix1, iy0, iy1;
   n0 = GradientNoise3D(x, y, z, x0, y0, z0, seed);
   n1 = GradientNoise3D(x, y, z, x1, y0, z0, seed);
   ix0 = LinearInterp(n0, n1, xs);
   n0 = GradientNoise3D(x, y, z, x0, y1, z0, seed);
   n1 = GradientNoise3D(x, y, z, x1, y1, z0, seed);
   ix1 = LinearInterp(n0, n1, xs);
   iy0 = LinearInterp(ix0, ix1, ys);

   n0 = GradientNoise3D(x, y, z, x0, y0, z1, seed);
   n1 = GradientNoise3D(x, y, z, x1, y0, z1, seed);
   ix0 = LinearInterp(n0, n1, xs);
   n0 = GradientNoise3D(x, y, z, x0, y1, z1, seed);
   n1 = GradientNoise3D(x, y, z, x1, y1, z1, seed);
   ix1 = LinearInterp(n0, n1, xs);
   iy1 = LinearInterp(ix0, ix1, ys);

   return LinearInterp(iy0, iy1, zs);
}

float ValueCoherentNoise3D(float x, float y, float z, int seed, int noiseQuality)
{


   int x0 = (x > 0.0 ? (int)(x) : (int)(x) - 1);
   int x1 = x0 + 1;
   int y0 = (y > 0.0 ? (int)(y) : (int)(y) - 1);
   int y1 = y0 + 1;
   int z0 = (z > 0.0 ? (int)(z) : (int)(z) - 1);
   int z1 = z0 + 1;



   float xs = 0, ys = 0, zs = 0;
   switch (noiseQuality) {
   case 0:
      xs = (x - (float)(x0));
      ys = (y - (float)(y0));
      zs = (z - (float)(z0));
      break;
   case 1:
      xs = SCurve3(x - (float)(x0));
      ys = SCurve3(y - (float)(y0));
      zs = SCurve3(z - (float)(z0));
      break;
   case 2:
      xs = SCurve5(x - (float)(x0));
      ys = SCurve5(y - (float)(y0));
      zs = SCurve5(z - (float)(z0));
      break;
   }





   float n0, n1, ix0, ix1, iy0, iy1;
   n0 = ValueNoise3D(x0, y0, z0, seed);
   n1 = ValueNoise3D(x1, y0, z0, seed);
   ix0 = LinearInterp(n0, n1, xs);
   n0 = ValueNoise3D(x0, y1, z0, seed);
   n1 = ValueNoise3D(x1, y1, z0, seed);
   ix1 = LinearInterp(n0, n1, xs);
   iy0 = LinearInterp(ix0, ix1, ys);
   n0 = ValueNoise3D(x0, y0, z1, seed);
   n1 = ValueNoise3D(x1, y0, z1, seed);
   ix0 = LinearInterp(n0, n1, xs);
   n0 = ValueNoise3D(x0, y1, z1, seed);
   n1 = ValueNoise3D(x1, y1, z1, seed);
   ix1 = LinearInterp(n0, n1, xs);
   iy1 = LinearInterp(ix0, ix1, ys);
   return LinearInterp(iy0, iy1, zs);
}
# 5 "Voxel_Lib/../../libnoise/libnoise.inc" 2
# 1 "Voxel_Lib/../../libnoise/libnoise_module.inc" 1
# 9 "Voxel_Lib/../../libnoise/libnoise_module.inc"
struct Module Perlin_GetValue(struct Module module)
{
   float x = module.m_X;
   float y = module.m_Y;
   float z = module.m_Z;

   float value = 0.0;
   float signal = 0.0;
   float curPersistence = 1.0;
   float nx, ny, nz;
   int seed;

   x *= module.m_frequency;
   y *= module.m_frequency;
   z *= module.m_frequency;

   for (int curOctave = 0; curOctave < module.m_octaveCount && curOctave < 30; curOctave++) {



      nx = MakeInt32Range(x);
      ny = MakeInt32Range(y);
      nz = MakeInt32Range(z);






      seed = (module.m_seed + curOctave) & 0xffffffff;
      signal = GradientCoherentNoise3D(nx, ny, nz, seed, module.m_noiseQuality);
      value += signal * curPersistence;


      x *= module.m_lacunarity;
      y *= module.m_lacunarity;
      z *= module.m_lacunarity;
      curPersistence *= module.m_persistence;
   }

   module.Value = value;
   return module;
}


struct Module Billow_GetValue(struct Module module)
{
   float x = module.m_X;
   float y = module.m_Y;
   float z = module.m_Z;

   float value = 0.0;
   float signal = 0.0;
   float curPersistence = 1.0;
   float nx, ny, nz;
   int seed;

   x *= module.m_frequency;
   y *= module.m_frequency;
   z *= module.m_frequency;

   for (int curOctave = 0; curOctave < module.m_octaveCount && curOctave < 30; curOctave++) {



      nx = MakeInt32Range(x);
      ny = MakeInt32Range(y);
      nz = MakeInt32Range(z);




      seed = (module.m_seed + curOctave) & 0xffffffff;
      signal = GradientCoherentNoise3D(nx, ny, nz, seed, module.m_noiseQuality);
      signal = 2.0f * fabs(signal) - 1.0f;
      value += signal * curPersistence;


      x *= module.m_lacunarity;
      y *= module.m_lacunarity;
      z *= module.m_lacunarity;
      curPersistence *= module.m_persistence;
   }
   value += 0.5;

   module.Value = value;
   return module;
}

struct Module RidgedMulti_GetValue(struct Module module)
{
   float x = module.m_X;
   float y = module.m_Y;
   float z = module.m_Z;

   x *= module.m_frequency;
   y *= module.m_frequency;
   z *= module.m_frequency;

   float signal = 0.0;
   float value = 0.0;
   float weight = 1.0;


   float specFreq = 1.0;
   float pSpectralWeight = 0;

   for (int curOctave = 0; curOctave < module.m_octaveCount && curOctave < 30; curOctave++) {



      float nx, ny, nz;
      nx = MakeInt32Range(x);
      ny = MakeInt32Range(y);
      nz = MakeInt32Range(z);


      int seed = (module.m_seed + curOctave) & 0x7fffffff;
      signal = GradientCoherentNoise3D(nx, ny, nz, seed, module.m_noiseQuality);


      signal = fabs(signal);
      signal = module.m_ridged_offset - signal;


      signal *= signal;




      signal *= weight;


      weight = signal * module.m_ridged_gain;
      if (weight > 1.0) {
         weight = 1.0;
      }
      if (weight < 0.0) {
         weight = 0.0;
      }


      pSpectralWeight = pow(specFreq, -module.m_ridged_H);
      specFreq *= module.m_lacunarity;

      value += (signal * pSpectralWeight);


      x *= module.m_lacunarity;
      y *= module.m_lacunarity;
      z *= module.m_lacunarity;
   }

   module.Value = (value * 1.25) - 1.0;
   return module;
}

struct Module Checkerboard_GetValue(struct Module module)
{
   int ix = (int)(floor(MakeInt32Range(module.m_X)));
   int iy = (int)(floor(MakeInt32Range(module.m_Y)));
   int iz = (int)(floor(MakeInt32Range(module.m_Z)));



   return module;
}

struct Module Cylinders_GetValue(struct Module module)
{
   float x = module.m_X;
   float y = module.m_Y;
   float z = module.m_Z;

   x *= module.m_frequency;
   z *= module.m_frequency;

   float distFromCenter = sqrt(x * x + z * z);
   float distFromSmallerSphere = distFromCenter - floor(distFromCenter);
   float distFromLargerSphere = 1.0 - distFromSmallerSphere;
   float nearestDist = min(distFromSmallerSphere, distFromLargerSphere);

   module.Value = 1.0 - (nearestDist * 4.0);
   return module;
}

struct Module Spheres_GetValue(struct Module module)
{
   float x = module.m_X;
   float y = module.m_Y;
   float z = module.m_Z;

   x *= module.m_frequency;
   y *= module.m_frequency;
   z *= module.m_frequency;

   float distFromCenter = sqrt(x * x + y * y + z * z);
   float distFromSmallerSphere = distFromCenter - floor(distFromCenter);
   float distFromLargerSphere = 1.0 - distFromSmallerSphere;
   float nearestDist = min(distFromSmallerSphere, distFromLargerSphere);

   module.Value = 1.0 - (nearestDist * 4.0);
   return module;
}

struct Module Voronoi_GetValue(struct Module module)
{
   float x = module.m_X;
   float y = module.m_Y;
   float z = module.m_Z;




   x *= module.m_frequency;
   y *= module.m_frequency;
   z *= module.m_frequency;

   int xInt = (x > 0.0 ? (int)(x) : (int)(x) - 1);
   int yInt = (y > 0.0 ? (int)(y) : (int)(y) - 1);
   int zInt = (z > 0.0 ? (int)(z) : (int)(z) - 1);

   float minDist = 2147483647.0;
   float xCandidate = 0;
   float yCandidate = 0;
   float zCandidate = 0;




   for (int zCur = zInt - 2; zCur <= zInt + 2; zCur++) {
      for (int yCur = yInt - 2; yCur <= yInt + 2; yCur++) {
         for (int xCur = xInt - 2; xCur <= xInt + 2; xCur++) {



            float xPos = xCur + ValueNoise3D(xCur, yCur, zCur, module.m_seed);
            float yPos = yCur + ValueNoise3D(xCur, yCur, zCur, module.m_seed + 1);
            float zPos = zCur + ValueNoise3D(xCur, yCur, zCur, module.m_seed + 2);
            float xDist = xPos - x;
            float yDist = yPos - y;
            float zDist = zPos - z;
            float dist = xDist * xDist + yDist * yDist + zDist * zDist;

            if (dist < minDist) {


               minDist = dist;
               xCandidate = xPos;
               yCandidate = yPos;
               zCandidate = zPos;
            }
         }
      }
   }

   float value;
   if (module.m_voronoi_enableDistance) {

      float xDist = xCandidate - x;
      float yDist = yCandidate - y;
      float zDist = zCandidate - z;
      value = (sqrt(xDist * xDist + yDist * yDist + zDist * zDist)
         ) * 1.7320508075688772935 - 1.0;
   }
   else {
      value = 0.0;
   }


   module.Value = value + (module.m_voronoi_displacement * (float)(ValueNoise3D( (int)(floor(xCandidate)), (int)(floor(yCandidate)), (int)(floor(zCandidate)), 0))


                                 );

   return module;
}





struct Module ABS_GetValue(struct Module module)
{
   module.Value = fabs(module.Value);
   return module;
}

struct Module Curve_GetValue(struct Module module, struct ControlPoint m_pControlPoints[10], int m_controlPointCount)
{

   float sourceModuleValue = module.Value;



   int indexPos;
   for (indexPos = 0; indexPos < m_controlPointCount; indexPos++) {
      if (sourceModuleValue < m_pControlPoints[indexPos].inputValue) {
         break;
      }
   }



   int index0 = ClampValue(indexPos - 2, 0, m_controlPointCount - 1);
   int index1 = ClampValue(indexPos - 1, 0, m_controlPointCount - 1);
   int index2 = ClampValue(indexPos, 0, m_controlPointCount - 1);
   int index3 = ClampValue(indexPos + 1, 0, m_controlPointCount - 1);





   if (index1 == index2) {
      module.Value = m_pControlPoints[index1].outputValue;
      return module;
   }


   float input0 = m_pControlPoints[index1].inputValue;
   float input1 = m_pControlPoints[index2].inputValue;
   float alpha = (sourceModuleValue - input0) / (input1 - input0);


   module.Value = CubicInterp(
      m_pControlPoints[index0].outputValue,
      m_pControlPoints[index1].outputValue,
      m_pControlPoints[index2].outputValue,
      m_pControlPoints[index3].outputValue,
      alpha);

   return module;
}

struct Module Exponent_GetValue(struct Module module, float m_exponent)
{
   float value = module.Value;
   float abs_val = fabs((value + 1.0f) / 2.0f);
   module.Value = (pow(abs_val, m_exponent) * 2.0f - 1.0f);
   return module;
}

struct Module Invert_GetValue(struct Module module)
{
   module.Value = -(module.Value);
   return module;
}

struct Module ScaleBias_GetValue(struct Module module, float m_scale, float m_bias)
{
   module.Value = module.Value * m_scale + m_bias;
   return module;
}





struct Module Displace_GetModule(struct Module module, struct Module dm_X, struct Module dm_Y, struct Module dm_Z)
{


   module.m_X = module.m_X + (dm_X.Value);
   module.m_Y = module.m_Y + (dm_Y.Value);
   module.m_Z = module.m_Z + (dm_Z.Value);



   return module;
}

struct Module RotatePoint_SetAngles(struct Module module, float angle_x, float angle_y, float angle_z)
{
   float xAngle = angle_x;
   float yAngle = angle_y;
   float zAngle = angle_z;

   float xCos;
   float yCos;
   float zCos;
   float xSin;
   float ySin;
   float zSin;

   xCos = cos(xAngle * 0.01745329);
   yCos = cos(yAngle * 0.01745329);
   zCos = cos(zAngle * 0.01745329);
   xSin = sin(xAngle * 0.01745329);
   ySin = sin(yAngle * 0.01745329);
   zSin = sin(zAngle * 0.01745329);

   module.m_x1Matrix = ySin * xSin * zSin + yCos * zCos;
   module.m_y1Matrix = xCos * zSin;
   module.m_z1Matrix = ySin * zCos - yCos * xSin * zSin;
   module.m_x2Matrix = ySin * xSin * zCos - yCos * zSin;
   module.m_y2Matrix = xCos * zCos;
   module.m_z2Matrix = -yCos * xSin * zCos - ySin * zSin;
   module.m_x3Matrix = -ySin * xCos;
   module.m_y3Matrix = xSin;
   module.m_z3Matrix = yCos * xCos;

   module.m_xAngle = xAngle;
   module.m_yAngle = yAngle;
   module.m_zAngle = zAngle;

   return module;
}

struct Module RotatePoint_GetModule_1(struct Module module)
{
   float x = module.m_X;
   float y = module.m_Y;
   float z = module.m_Z;

   module.m_X = (module.m_x1Matrix * x) + (module.m_y1Matrix * y) + (module.m_z1Matrix * z);
   module.m_Y = (module.m_x2Matrix * x) + (module.m_y2Matrix * y) + (module.m_z2Matrix * z);
   module.m_Z = (module.m_x3Matrix * x) + (module.m_y3Matrix * y) + (module.m_z3Matrix * z);

   return module;
}

struct Module RotatePoint_GetModule_2(struct Module module, float angle_x, float angle_y, float angle_z)
{
   module = RotatePoint_SetAngles(module, angle_x, angle_y, angle_z);
   module = RotatePoint_GetModule_1(module);

   return module;
}

struct Module ScalePoint_GetModule(struct Module module, float m_scale_x, float m_scale_y, float m_scale_z)
{
   module.m_X *= m_scale_x;
   module.m_Y *= m_scale_y;
   module.m_Z *= m_scale_z;

   return module;
}

struct Module Select_SetEdgeFalloff(struct Module module, float edgeFalloff)
{

   float boundSize = module.m_upperBound - module.m_lowerBound;
   module.m_edgeFalloff = (edgeFalloff > boundSize / 2) ? boundSize / 2 : edgeFalloff;

   return module;
}

struct Module Select_SetBounds(struct Module module, float lowerBound, float upperBound)
{
   module.m_lowerBound = lowerBound;
   module.m_upperBound = upperBound;


   Select_SetEdgeFalloff(module, module.m_edgeFalloff);

   return module;
}

struct Module TranslatePoint_GetModule(struct Module module, float m_Translation_x, float m_Translation_y, float m_Translation_z)
{
   module.m_X += m_Translation_x;
   module.m_Y += m_Translation_y;
   module.m_Z += m_Translation_z;

   return module;
}

struct Module Turbulence_GetValue(struct Module module)
{
   float x = module.m_X;
   float y = module.m_Y;
   float z = module.m_Z;
# 489 "Voxel_Lib/../../libnoise/libnoise_module.inc"
   float x0, y0, z0;
   float x1, y1, z1;
   float x2, y2, z2;
   x0 = x + (12414.0 / 65536.0);
   y0 = y + (65124.0 / 65536.0);
   z0 = z + (31337.0 / 65536.0);
   x1 = x + (26519.0 / 65536.0);
   y1 = y + (18128.0 / 65536.0);
   z1 = z + (60493.0 / 65536.0);
   x2 = x + (53820.0 / 65536.0);
   y2 = y + (11213.0 / 65536.0);
   z2 = z + (44845.0 / 65536.0);

   struct Module m_DistortModule = GetModule_3(x0, y0, z0);
   m_DistortModule.m_seed = (int)(module.m_turb_seed);
   m_DistortModule.m_frequency = module.m_turb_frequency;
   m_DistortModule.m_octaveCount = module.m_turb_roughness;
   float xDistort = x + (Perlin_GetValue(m_DistortModule).Value * module.m_turb_power);

   m_DistortModule = GetModule_3(x1, y1, z1);
   m_DistortModule.m_seed = (int)(module.m_turb_seed) + 1;
   m_DistortModule.m_frequency = module.m_turb_frequency;
   m_DistortModule.m_octaveCount = module.m_turb_roughness;
   float yDistort = y + (Perlin_GetValue(m_DistortModule).Value * module.m_turb_power);

   m_DistortModule = GetModule_3(x2, y2, z2);
   m_DistortModule.m_seed = (int)(module.m_turb_seed) + 2;
   m_DistortModule.m_frequency = module.m_turb_frequency;
   m_DistortModule.m_octaveCount = module.m_turb_roughness;
   float zDistort = z + (Perlin_GetValue(m_DistortModule).Value * module.m_turb_power);



   module.m_X = xDistort;
   module.m_Y = yDistort;
   module.m_Z = zDistort;

   return module;
}




float Blend_GetValue(struct Module module_1, struct Module module_2, struct Module module_alpha)
{
   float v0 = module_1.Value;
   float v1 = module_2.Value;
   float alpha = (module_alpha.Value + 1.0) / 2.0;

   return LinearInterp(v0, v1, alpha);
}

float Max_GetValue(struct Module module_1, struct Module module_2)
{
   float v0 = module_1.Value;
   float v1 = module_2.Value;
   return max(v0, v1);
}

float Min_GetValue(struct Module module_1, struct Module module_2)
{
   float v0 = module_1.Value;
   float v1 = module_2.Value;
   return min(v0, v1);
}

float Add_GetValue(struct Module module_1, struct Module module_2)
{
   return module_1.Value + module_2.Value;
}

float Multiply_GetValue(struct Module module_1, struct Module module_2)
{
   return module_1.Value * module_2.Value;
}

float Subtract_GetValue(struct Module module_1, struct Module module_2)
{
   return module_1.Value - module_2.Value;
}

float Divide_GetValue(struct Module module_1, struct Module module_2)
{
   return module_1.Value / module_2.Value;
}


float Power_GetValue(struct Module module_1, struct Module module_2)
{
   return pow(module_1.Value, module_2.Value);
}

float Select_GetValue(struct Module module_1, struct Module module_2, struct Module control_mod)
{
   float value;

   float controlValue = control_mod.Value;
   float m_edgeFalloff = control_mod.m_edgeFalloff;
   float m_lowerBound = control_mod.m_lowerBound;
   float m_upperBound = control_mod.m_upperBound;


   float alpha;
   if (m_edgeFalloff > 0.0) {
      if (controlValue < (m_lowerBound - m_edgeFalloff)) {


         value = module_1.Value;

      }
      else if (controlValue < (m_lowerBound + m_edgeFalloff)) {



         float lowerCurve = (m_lowerBound - m_edgeFalloff);
         float upperCurve = (m_lowerBound + m_edgeFalloff);
         alpha = SCurve3((controlValue - lowerCurve) / (upperCurve - lowerCurve));

         value = LinearInterp(module_1.Value, module_2.Value, alpha);

      }
      else if (controlValue < (m_upperBound - m_edgeFalloff)) {


         value = module_2.Value;

      }
      else if (controlValue < (m_upperBound + m_edgeFalloff)) {



         float lowerCurve = (m_upperBound - m_edgeFalloff);
         float upperCurve = (m_upperBound + m_edgeFalloff);
         alpha = SCurve3((controlValue - lowerCurve) / (upperCurve - lowerCurve));

         value = LinearInterp(module_2.Value, module_1.Value, alpha);

      }
      else {


         value = module_1.Value;
      }
   }
   else {
      if (controlValue < m_lowerBound || controlValue > m_upperBound) {
         value = module_1.Value;
      }
      else {
         value = module_2.Value;
      }
   }

   return value;
}
# 6 "Voxel_Lib/../../libnoise/libnoise.inc" 2
# 1 "Voxel_Lib/../../libnoise/libnoise_model.inc" 1





struct Module Model_Cylinder_GetModule(struct Module m_pModule, float angle, float height)
{
   m_pModule.m_X = cos(angle * 0.01745329);
   m_pModule.m_Y = height;
   m_pModule.m_Z = sin(angle * 0.01745329);
   return m_pModule;
}

struct Module Model_Line_GetModule(struct Module m_pModule, float3 start, float3 end, float p)
{
   m_pModule.m_X = (end.x - start.x) * p + start.x;
   m_pModule.m_Y = (end.y - start.y) * p + start.y;
   m_pModule.m_Z = (end.z - start.z) * p + start.z;

   return m_pModule;
}

float Model_Line_m_Attenuate(struct Module m_pModule, float p)
{
   m_pModule.Value = p * (1.0 - p) * 4 * m_pModule.Value;
   return m_pModule.Value;
}

struct Module Sphere_GetValue(struct Module m_pModule, float lat, float lon)
{
   float3 res = LatLonToXYZ(lat, lon);

   m_pModule.m_X = res.x;
   m_pModule.m_Y = res.y;
   m_pModule.m_Z = res.z;

   return m_pModule;
}
# 7 "Voxel_Lib/../../libnoise/libnoise.inc" 2
# 6 "Voxel_Lib/voxel_lib_core.inc" 2

struct Static_Settings{
    float4 VoxelsPerMeter;
    uint4 ChunkMeterSize;
    uint4 ChunkSize;
    uint4 FullChunkSize;
    float4 skipDist;
    float4 half_;
    float4 SideLength;
    uint4 batches;
};

struct Run_Settings{
    int4 Location;
};

struct Materials{
    int4 material_types;
    float4 material_ratios;
};

struct ISO_Material{
    float4 final_iso;
    int4 material_types;
    float4 material_ratios;
};

struct GridPoint{
 float iso;
    float4 localPosition;
 float4 globalPosition;
 int3 voxelIndex;
    float4 normal;
    int4 type;
    float4 type_ratio;
};

struct Polygon{
 float4 vertices[3];
 float4 normal;
};







struct Polygon New_Polygon(float4 v1, float4 v2, float4 v3){
 struct Polygon res;
 res.vertices[0] = v1;
 res.vertices[1] = v2;
 res.vertices[2] = v3;
 res.normal = ((float4)(normalize(cross((v2 - v1).xyz, (v3 - v1).xyz)).x, normalize(cross((v2 - v1).xyz, (v3 - v1).xyz)).y, normalize(cross((v2 - v1).xyz, (v3 - v1).xyz)).z, 0));
 return res;
}

struct Materials New_Materials(){
 struct Materials mat;
 mat.material_types = (int4)(0, 0, 0, 0);
 mat.material_ratios = (float4)(0, 0, 0, 0);
 return mat;
}

struct ISO_Material New_ISO_Material(){
 struct ISO_Material mat;
 mat.final_iso = (float4)(0, 0, 0, 0);
 mat.material_types = (int4)(0, 0, 0, 0);
 mat.material_ratios = (float4)(0, 0, 0, 0);
 return mat;
}

struct GridPoint New_GridPoint(){
 struct GridPoint res;
 res.iso = 0;
 res.localPosition = (float4)(0, 0, 0, 0);
 res.globalPosition = (float4)(0, 0, 0, 0);
 res.normal = (float4)(0, 0, 0, 0);
 res.type = (int4)(0, 0, 0, 0);
 res.type_ratio = (float4)(0, 0, 0, 0);
 return res;
}

float Scale(float value, float oldMin, float oldMax, float newMin, float newMax)
{
    return newMin + (value - oldMin) * (newMax - newMin) / (oldMax - oldMin);
}

uint GetVoxelIndex(uint batch, uint g_index, uint size){
    return (g_index % size);
}

uint GetBatchIndex(uint batch, uint v_index, uint size){
    return (batch * size) + v_index;
}

int3 C_1D_to_3D(uint i, uint width, uint height) {
 int z = (int)(i / (width * height));
 int y = (int)((i % (width * height)) / width);
 int x = (int)(i % width);

 return (int3)(x, y, z);
}

uint C_3D_to_1D(int x, int y, int z, uint width, uint height) {
 return (uint)(z * width * height + y * width + x);
}

uint C_2D_to_1D(int x, int y, uint width) {
    return (uint)(y * width + x);
}

int2 C_1D_to_2D(uint i, uint width) {
    int y = (int)(i / width);
    int x = (int)(i % width);

    return (int2)(x, y);
}
# 5 "Voxel_Lib/smoothrender_mark_lib.inc" 2

__global struct Static_Settings* in_static_settings;

__global int* in_counts_data;

__global int* out_stitch_map_data;
__global int4* out_counts_data;

__global float4* out_debug_data;


void SetStitchMap(uint voxel_index, int poly_buffer_index){
 out_stitch_map_data[voxel_index] = poly_buffer_index;
}

int GetCount(uint index){
 return in_counts_data[index];
}

void Set_Debug_Data(uint index, float4 data){
 out_debug_data[index] = data;
}

void CreateStitchMap(uint batch_index, uint batchesPerGroup, uint fullChunkSize){

    uint batch_offset = (batch_index) * fullChunkSize;

    int buffer_index = 0;
 uint v_index = 0;
    for (v_index = 0; v_index < fullChunkSize; ++v_index){

        int poly_count = GetCount(batch_offset + v_index);

        if (poly_count == 0){
            continue;
        }

        SetStitchMap(batch_offset + v_index, buffer_index);

        buffer_index += poly_count;
    }

 out_counts_data[batch_index] = (int4)(buffer_index, 0, batch_index, 0);

}

void smoothrender_mark_main(uint batch_index){

    struct Static_Settings static_settings = in_static_settings[0];



    CreateStitchMap(batch_index, static_settings.batches[0], (uint)(static_settings.FullChunkSize[0]));



}
# 6 "5-smoothrender_mark.cl" 2

void kernel main_cl(
 global struct Static_Settings* p_in_static_settings,

 global int* p_in_counts_data,

 global int* p_out_stitch_map_data,
 global int4* p_out_counts_data,

 global float4* p_out_debug_data,
)
{
 in_static_settings = p_in_static_settings;
 in_counts_data = p_in_counts_data;
 out_stitch_map_data = p_out_stitch_map_data;
 out_counts_data = p_out_counts_data;
 out_debug_data = p_out_debug_data;


 smoothrender_mark_main(get_global_id(0));
}
