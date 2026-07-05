# 1. Получаем все CSV файлы, исключая результаты
$allFiles = Get-ChildItem -Filter "*.csv" | 
            Where-Object { $_.Name -notlike "*_output.csv" } | 
            Select-Object -ExpandProperty Name

if ($null -eq $allFiles -or $allFiles.Count -eq 0) {
    Write-Host "CSV-файлы для обработки не найдены." -ForegroundColor Yellow
    Exit
}

# 2. Разделяем файлы на две группы с помощью фильтра
$cmFiles = $allFiles | Where-Object { $_ -like "cm*" -or $_ -like "цм*" }
$regularFiles = $allFiles | Where-Object { $_ -notlike "cm*" -and $_ -notlike "цм*" }

# 3. Запуск парсера для файлов ЦМ / CM
if ($null -ne $cmFiles -and $cmFiles.Count -gt 0) {
    Write-Host "Обработка файлов ЦМ/CM (с флагом -cm)..." -ForegroundColor Green
    .\parser_rep.exe `
        -cm `
        $cmFiles
}

# 4. Запуск парсера для обычных файлов
if ($null -ne $regularFiles -and $regularFiles.Count -gt 0) {
    Write-Host "Обработка обычных файлов (без флага)..." -ForegroundColor Green
    .\parser_rep.exe `
        $regularFiles
