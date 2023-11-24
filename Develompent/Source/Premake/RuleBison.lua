-- Output directory for Bison
bisonOutputDir       = "%{wks.location}/Generated/%{prj.name}/"

rule "Bison"
    display         "Bison scripts custom compiler"
    fileExtension   ".bison"
    location( "../../Intermediate/" .. _ACTION .. "/" )

	propertyDefinition {
		name    = "AdditionalArguents",
		kind    = "string",
		value   = "",
	}

	buildcommands {
        'SET PATH=%{wks.location}/../../Tools/Bison/bin;%PATH%',
        'bison.exe [AdditionalArguents] --defines=' .. bisonOutputDir .. '"%{file.basename}_Bison.cpp.h" -o' .. bisonOutputDir .. '"%{file.basename}_Bison.cpp" "%{file.abspath}"'
    }

    buildoutputs {
        bisonOutputDir .. '%{file.basename}_Bison.cpp.h', 
        bisonOutputDir .. '%{file.basename}_Bison.cpp'
    }