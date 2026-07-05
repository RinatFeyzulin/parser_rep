# 1. Автоматическая конвертация .xlsx в .csv через Microsoft Excel
$xlsxFiles = Get-ChildItem -Filter "*.xlsx" | Select-Object -ExpandProperty FullName

if ($null -ne $xlsxFiles -and $xlsxFiles.Count -gt 0) {
    Write-Host "Найдена таблица(ы) XLSX. Запускаю Microsoft Excel для конвертации..." -ForegroundColor Cyan
    
    # Создаем скрытый объект Excel в памяти
    $excel = New-Object -ComObject Excel.Application
    $excel.Visible = $false
    $excel.DisplayAlerts = $false

    foreach ($xlsx in $xlsxFiles) {
        # Формируем путь для нового .csv файла (меняем расширение)
        $csvPath = $xlsx -replace "\.xlsx$", ".csv"
        
        # Открываем книгу XLSX
        $workbook = $excel.Workbooks.Open($xlsx)
        
        # Переключаем Excel в локальный режим, чтобы он использовал системный разделитель (в РФ это точка с запятой)
        # xlCSV = 6 (стандартный CSV)
        $workbook.SaveAs($csvPath, 6, [Type]::Missing, [Type]::Missing, [Type]::Missing, [Type]::Missing, [Type]::Missing, [Type]::Missing, [Type]::Missing, [Type]::Missing, [Type]::Missing, $true)
        
        $workbook.Close($false)
        Write-Host "Сконвертировано: $(Split-Path $xlsx -Leaf) -> $(Split-Path $csvPath -Leaf)" -ForegroundColor Cyan
    }

    # Закрываем Excel и очищаем память
    $excel.Quit()
    [System.Runtime.InteropServices.Marshal]::ReleaseComObject($excel) | Out-Null
    Remove-Variable excel
}

# 2. Получаем все CSV файлы (включая только что созданные), исключая результаты
$allFiles = Get-ChildItem -Filter "*.csv" | 
            Where-Object { $_.Name -notlike "*_output.csv" } | 
            Select-Object -ExpandProperty Name

if ($null -eq $allFiles -or $allFiles.Count -eq 0) {
    Write-Host "CSV-файлы для обработки не найдены." -ForegroundColor Yellow
    Exit
}

# 3. Разделяем файлы на две группы с помощью фильтра
$cmFiles = $allFiles | Where-Object { $_ -like "cm*" -or $_ -like "цм*" }
$regularFiles = $allFiles | Where-Object { $_ -notlike "cm*" -and $_ -notlike "цм*" }

# 4. Запуск парсера для файлов ЦМ / CM
if ($null -ne $cmFiles -and $cmFiles.Count -gt 0) {
    Write-Host "Обработка файлов ЦМ/CM (с флагом -cm)..." -ForegroundColor Green
    .\my_parser.exe `
        -cm `
        $cmFiles
}

# 5. Запуск парсера для обычных файлов
if ($null -ne $regularFiles -and $regularFiles.Count -gt 0) {
    Write-Host "Обработка обычных файлов (без флага)..." -ForegroundColor Green
    .\my_parser.exe `
        $regularFiles
}

Write-Host "Все файлы успешно обработаны!" -ForegroundColor Green
