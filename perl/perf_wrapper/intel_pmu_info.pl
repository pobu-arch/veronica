#!/usr/bin/env perl

%GEEKBENCH_INFO =
(
####################################################################################################

# Geekbench 4

####################################################################################################

    '101.AES' =>
    {
        'version'   => '4',
        'iteration' => '4000',
        'type'      => 'crypto'
    },

    '201.LZMA' =>
    {
        'version'   => '4',
        'iteration' => '1600',
        'type'      => 'int'
    },

    '202.JPEG' =>
    {
        'version'   => '4',
        'iteration' => '1600',
        'type'      => 'int'
    },

    '204.Canny' =>
    {
        'version'   => '4',
        'iteration' => '2400',
        'type'      => 'int'
    },

    '205.Lua' =>
    {
        'version'   => '4',
        'iteration' => '3600',
        'type'      => 'int'
    },

    '206.Dijkstra' =>
    {
        'version'   => '4',
        'iteration' => '2000',
        'type'      => 'int'
    },

    '207.SQLite' =>
    {
        'version'   => '4',
        'iteration' => '1600',
        'type'      => 'int'
    },

    '208.HTML5_Parse' =>
    {
        'version'   => '4',
        'iteration' => '4000',
        'type'      => 'int'
    },

#    this one has memory allocation error when it'srunning with >5 iterations
#    '209.HTML5_DOM' =>
#    {
#        'version'   => '4',
#        'iteration' => '5',
#        'type'      => 'int'
#    },

    '210.Histogram_Equalization' =>
    {
        'version'   => '4',
        'iteration' => '2800',
        'type'      => 'int'
    },

    '211.PDF_Rendering' =>
    {
        'version'   => '4',
        'iteration' => '1200',
        'type'      => 'int'
    },

    '212.LLVM' =>
    {
        'version'   => '4',
        'iteration' => '1200',
        'type'      => 'int'
    },

    '213.Camera' =>
    {
        'version'   => '4',
        'iteration' => '2400',
        'type'      => 'int'
    },

    '301.SGEMM' =>
    {
        'version'   => '4',
        'iteration' => '4000',
        'type'      => 'fp'
    },

    '302.SFFT' =>
    {
        'version'   => '4',
        'iteration' => '6000',
        'type'      => 'fp'
    },

    '303.N_Body_Physics' =>
    {
        'version'   => '4',
        'iteration' => '1200',
        'type'      => 'fp'
    },

    '304.Ray_Tracing' =>
    {
        'version'   => '4',
        'iteration' => '800',
        'type'      => 'fp'
    },

    '305.Rigid_Body_Physics' =>
    {
        'version'   => '4',
        'iteration' => '2000',
        'type'      => 'fp'
    },

    '306.HDR' =>
    {
        'version'   => '4',
        'iteration' => '1000',
        'type'      => 'fp'
    },

    '307.Gaussian_Blur' =>
    {
        'version'   => '4',
        'iteration' => '1600',
        'type'      => 'fp'
    },

    '308.Speech_Recognition' =>
    {
        'version'   => '4',
        'iteration' => '800',
        'type'      => 'fp'
    },

    '309.Face_Detection' =>
    {
        'version'   => '4',
        'iteration' => '1600',
        'type'      => 'fp'
    },

    '401.Memory_Copy' =>
    {
        'version'   => '4',
        'iteration' => '400',
        'type'      => 'mem'
    },

    '402.Memory_Latency' =>
    {
        'version'   => '4',
        'iteration' => '800',
        'type'      => 'mem'
    },

    '403.Memory_Bandwidth' =>
    {
        'version'   => '4',
        'iteration' => '500',
        'type'      => 'mem'
    },

####################################################################################################

# Geekbench 5

####################################################################################################

    '101.AES-XTS' =>
    {
        'version'   => '5',
        'iteration' => '4000',
        'type'      => 'crypto',
    },

    '201.Text_Compression' =>
    {
        'version'   => '5',
        'iteration' => '350',
        'type'      => 'int',
    },

    '202.Image_Compression' =>
    {
        'version'   => '5',
        'iteration' => '900',
        'type'      => 'int'
    },

    '203.Navigation' =>
    {
        'version'   => '5',
        'iteration' => '800',
        'type'      => 'int'
    },

    '204.HTML5' =>
    {
        'version'   => '5',
        'iteration' => '3200',
        'type'      => 'int'
    },

    '205.SQLite' =>
    {
        'version'   => '5',
        'iteration' => '2400',
        'type'      => 'int'
    },

    '206.PDF_Rendering' =>
    {
        'version'   => '5',
        'iteration' => '1000',
        'type'      => 'int'
    },

    '207.Text_Rendering' =>
    {
        'version'   => '5',
        'iteration' => '5000',
        'type'      => 'int'
    },

    '208.Clang' =>
    {
        'version'   => '5',
        'iteration' => '1200',
        'type'      => 'int'
    },

    '209.Camera' =>
    {
        'version'   => '5',
        'iteration' => '2400',
        'type'      => 'int'
    },

    '301.N_Body_Physics' =>
    {
        'version'   => '5',
        'iteration' => '400',
        'type'      => 'fp'
    },

    '302.Rigid_Body_Physics' =>
    {
        'version'   => '5',
        'iteration' => '1000',
        'type'      => 'fp'
    },

    '303.Gaussian_Blur' =>
    {
        'version'   => '5',
        'iteration' => '450',
        'type'      => 'fp'
    },

    '305.Face_Detection' =>
    {
        'version'   => '5',
        'iteration' => '1000',
        'type'      => 'fp'
    },

    '306.Horizon_Detection' =>
    {
        'version'   => '5',
        'iteration' => '550',
        'type'      => 'fp'
    },

    '307.Image_Inpainting' =>
    {
        'version'   => '5',
        'iteration' => '1600',
        'type'      => 'fp'
    },

    '308.HDR' =>
    {
        'version'   => '5',
        'iteration' => '260',
        'type'      => 'fp'
    },

    '309.Ray_Tracing' =>
    {
        'version'   => '5',
        'iteration' => '2000',
        'type'      => 'fp'
    },

    '310.Structure_from_Motion' =>
    {
        'version'   => '5',
        'iteration' => '300',
        'type'      => 'fp'
    },

    '312.Speech_Recognition' =>
    {
        'version'   => '5',
        'iteration' => '700',
        'type'      => 'fp'
    },

    '313.Machine_Learning' =>
    {
        'version'   => '5',
        'iteration' => '8000',
        'type'      => 'fp'
    }
);
1