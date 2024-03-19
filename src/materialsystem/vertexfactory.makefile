{
	// Makefile type (either shader or vertexfactory)
	"makefile_type":					"vertexfactory",
	
	// Output path for generated C++ classes
	"cppgen_output": 					"generated/",
	
	// Vertex factories list
	"vertex_factories": [
		{
			"type": 					"simpleelement",
			"source":					"vf_simpleelement.hlsl",
			"supports_instancing":		false,
			"instance_stream_index":	0,
			"defines":	[
				{ "name": "TEST_DEF", "value": "1" }
			]
		}
	]
}