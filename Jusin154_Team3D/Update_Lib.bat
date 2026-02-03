
xcopy			/y /s	.\Engine\Public\*.*					.\EngineSDK\Inc\

xcopy			/y		.\Engine\Bin\Debug\Engine.dll				.\Client\Bin\Debug\
xcopy			/y		.\Engine\Bin\Release\Engine.dll				.\Client\Bin\Release\
xcopy			/y		.\Engine\Bin\Debug\Engine.dll				.\Editor\Bin\Debug\
xcopy			/y		.\Engine\Bin\Release\Engine.dll				.\Editor\Bin\Release\
xcopy			/y		.\Engine\Bin\Resources\ShaderFiles\*		.\Client\Bin\Resources\ShaderFiles\
xcopy			/y		.\Engine\Bin\Resources\ShaderFiles\*		.\Editor\Bin\Resources\ShaderFiles\

xcopy			/y		.\Engine\Bin\Debug\Engine.lib				.\EngineSDK\Lib\Debug\
xcopy			/y		.\Engine\Bin\Release\Engine.lib				.\EngineSDK\Lib\Release\


:: fmod
xcopy			/y		.\Engine\Bin\fMod\fMod.dll					.\Client\Bin\Debug\
xcopy			/y		.\Engine\Bin\fMod\fMod.dll					.\Editor\Bin\Debug\
xcopy			/y		.\Engine\Bin\fMod\fMod.dll					.\Client\Bin\Release\
xcopy			/y		.\Engine\Bin\fMod\fMod.dll					.\Editor\Bin\Release\

:: PhysX
xcopy			/y 		.\Engine\Bin\PhysX\PhysX_debug\*.dll				.\Editor\Bin\debug\
xcopy			/y 		.\Engine\Bin\PhysX\PhysX_release\*.dll				.\Editor\Bin\release\
xcopy			/y 		.\Engine\Bin\PhysX\PhysX_debug\*.dll				.\Client\Bin\debug\
xcopy			/y 		.\Engine\Bin\PhysX\PhysX_release\*.dll				.\Client\Bin\release\

:: assimp
:: engine에서 assimp의 dll을 읽어오기 때문에 client에서 사실상 사용은 안하더라도
:: 클라에서도 assimp.dll이 필요함
xcopy			/y		.\Engine\Bin\Assimp\assimp-vc145-mtd.dll	.\Client\Bin\Debug\
xcopy			/y		.\Engine\Bin\Assimp\assimp-vc145-mt.dll		.\Client\Bin\Release\
xcopy			/y		.\Engine\Bin\Assimp\assimp-vc145-mtd.dll	.\Editor\Bin\Debug\
xcopy			/y		.\Engine\Bin\Assimp\assimp-vc145-mt.dll		.\Editor\Bin\Release\

:: 동적링크라이브러리는 pdb랑 dll이랑 같이 있으면 클라에서 터졌을 때 매칭해서 볼 수 있음
:: fMod는 아쉽게도 pdb가 없기 때문에 안옮김
xcopy			/y	.\Engine\ThirdPartyLib\Assimp\assimp-vc145-mtd.pdb		.\Client\Bin\Debug\
xcopy			/y	.\Engine\ThirdPartyLib\Assimp\assimp-vc145-mt.pdb		.\Client\Bin\Release\
xcopy			/y	.\Engine\ThirdPartyLib\PhysX_debug\*.pdb				.\Client\Bin\Debug\
xcopy			/y	.\Engine\ThirdPartyLib\PhysX_release\*.pdb				.\Client\Bin\Release\
xcopy			/y	.\Engine\ThirdPartyLib\Assimp\assimp-vc145-mtd.pdb		.\Editor\Bin\Debug\
xcopy			/y	.\Engine\ThirdPartyLib\Assimp\assimp-vc145-mt.pdb		.\Editor\Bin\Release\
xcopy			/y	.\Engine\ThirdPartyLib\PhysX_debug\*.pdb				.\Editor\Bin\Debug\
xcopy			/y	.\Engine\ThirdPartyLib\PhysX_release\*.pdb				.\Editor\Bin\Release\
xcopy			/y	.\Editor\ThirdPartyLib\ZLib\Debug\zlibd.pdb				.\Editor\Bin\Debug\
xcopy			/y	.\Editor\ThirdPartyLib\ZLib\Release\zlib.pdb			.\Editor\Bin\Release\

xcopy			/y		.\Engine\Bin\Debug\Engine.pdb				.\Client\Bin\Debug\
xcopy			/y		.\Engine\Bin\Release\Engine.pdb				.\Client\Bin\Release\
xcopy			/y		.\Engine\Bin\Debug\Engine.pdb				.\Editor\Bin\Debug\
xcopy			/y		.\Engine\Bin\Release\Engine.pdb				.\Editor\Bin\Release\

:: 클라에서 제3자 라이브러리를 사용할 수도 있으니까 dll은 lib도 제공
:: 정적링크라이브러리는 엔진에 섞여서 내보내짐
xcopy			/y		.\Engine\ThirdPartyLib\Assimp\assimp-vc145-mtd.lib				.\EngineSDK\Lib\Debug\
xcopy			/y		.\Engine\ThirdPartyLib\Assimp\assimp-vc145-mt.lib				.\EngineSDK\Lib\Release\

xcopy			/y		.\Engine\ThirdPartyLib\fMod\fmod_vc.lib				.\EngineSDK\Lib\Debug\
xcopy			/y		.\Engine\ThirdPartyLib\fMod\fmod_vc.lib				.\EngineSDK\Lib\Release\

xcopy			/y		.\Engine\ThirdPartyLib\DirectXTK\DirectXTKd.lib				.\EngineSDK\Lib\Debug\
xcopy			/y		.\Engine\ThirdPartyLib\DirectXTK\DirectXTK.lib				.\EngineSDK\Lib\Release\
xcopy			/y		.\Engine\ThirdPartyLib\PhysX\PhysX_debug\*.lib				.\EngineSDK\Lib\Debug\
xcopy			/y		.\Engine\ThirdPartyLib\PhysX\PhysX_release\*.lib			.\EngineSDK\Lib\Release\
