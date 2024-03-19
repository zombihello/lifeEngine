{
	// Makefile type (either shader or vertexfactory)
	"makefile_type":					"shader",
	
	// Output path for generated C++ classes
	"cppgen_output": 					"generated/",
	
	// Include directories
	"include": [
		"../",
		"../../public/materialsystem/"
	],
	
	// Set vertex factories
	"vertex_factories": 				"../vertexfactory.makefile",
	
	"shaders": [
		// Unlit Generic
		{
			"source": 					"unlitgeneric_vs.hlsl",
			"use_vertex_factory": 		true,
			"vertex_factories": 		[ "simpleelement" ],
			"main_func": 				"MainVS",
			"type":						"vertex"
		},
		{
			"source": 					"unlitgeneric_ps.hlsl",
			"main_func": 				"MainPS",
			"type":						"pixel",
			"flags": [
				{ "name": "FILL_RED_COLOR", 	"min": 0, "max": 1, "default": 0 },
				{ "name": "USE_GREEN_CHANNEL", 	"min": 0, "max": 1, "default": 0 }
			],
			"defines": [
				{ "name": "TEST_MACRO", "value": "0" }
			]
		}
	]
}