"C:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" --bin ".\app_obj\application.axf"  --output firmware\app
"C:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" --bincombined --bincombined_padding=1,0x1A ".\app_obj\application.axf"  --output firmware\app\FULL_EROM
"checkSumGenerator.exe" firmware\app\FULL_EROM firmware\app\DESC_EROM