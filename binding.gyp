{
	"targets" : [{
			"target_name" : "OpenVEIL",
			"sources" : ["OpenVEILAddon.cpp", "OpenVEILAddon.h", "addon.cc"],
			"include_dirs" : ["c:/tecsec/OpenVEIL_7-0/vc14/include"],
			'conditions' : [
				['OS=="win"', {
						'cflags' : [
							'/EHsc',
							'/GR',
						],
						'cflags_cc!': [ '-fno-rtti' ],
						'link_settings': {
                            'libraries': ['c:/tecsec/OpenVEIL_7-0/vc14/lib/OpenVEILCore.lib'], 
                        },
            'msvs_settings': { 
               'VCCLCompilerTool': { 
                 'ExceptionHandling': '2', 
                 'DisableSpecificWarnings': [ '4530', '4506' ], 
               }, 
			},
					}, {
						 # OS != "win"
						'cflags' : [
							'-Werror',
						]
					}
				]
			]
		}
	]
}
