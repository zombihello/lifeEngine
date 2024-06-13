-- Output directory for Flex
flexOutputDir       = "%{wks.location}/Generated/%{prj.name}/"

rule "Flex"
    display         "Flex scripts custom compiler"
    fileExtension   ".flex"
    location( "../../Intermediate/" .. _ACTION .. "/" )

	propertyDefinition {
		name    = "AdditionalArguents",
		kind    = "string",
		value   = "",
	}

	buildcommands {
        'SET PATH=%{wks.location}/../../Tools/Flex/bin;%PATH%',
        'win_flex.exe [AdditionalArguents] -o"' .. flexOutputDir .. '%{file.basename}_Flex.cpp" "%{file.abspath}"'
    }

    buildoutputs {
        flexOutputDir .. '%{file.basename}_Flex.cpp'
    }