
rmdir	.\EngineSDK\Inc\				/s /q
rmdir	.\EngineSDK\Lib\Debug\			/s /q
rmdir	.\EngineSDK\Lib\Release\		/s /q

rmdir	.\Engine\Default\x64\			/s /q
rmdir	.\Client\Default\x64\			/s /q
rmdir	.\Editor\Default\x64\			/s /q

rmdir	.\Engine\Bin\Debug\				/s /q
rmdir	.\Client\Bin\Debug\				/s /q
rmdir	.\Editor\Bin\Debug\				/s /q

rmdir	.\Engine\Bin\Release\			/s /q
rmdir	.\Client\Bin\Release\			/s /q
rmdir	.\Editor\Bin\Release\			/s /q
rmdir	.\.vs\							/s /q


:: 정적라이브러리의 pdb는 심볼만 가져올 수 있도록 x64에 섞어서 보냄
:: 그러면 디버깅할 때 해당 라이브러리의 내부에서 터졌을 때 함수주소를 이름이랑 매칭해서 볼 수 있음
:: 안그러면 내부에서 터지면 함수 주소만 보임
xcopy			/y	.\Engine\ThirdPartyLib\DirectXTK\DirectXTKd.pdb					.\Engine\Default\x64\Debug\
xcopy			/y	.\Engine\ThirdPartyLib\DirectXTK\DirectXTK.pdb					.\Engine\Default\x64\Release\

xcopy			/y	.\Engine\ThirdPartyLib\FX11\Effects11d.pdb						.\Engine\Default\x64\Debug\
xcopy			/y	.\Engine\ThirdPartyLib\FX11\Effects11.pdb						.\Engine\Default\x64\Release\
