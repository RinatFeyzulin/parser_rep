#!/bin/bash

# 1. Компилируем, если нужно
if [ ! -f "./parser_rep" ]; then
    gcc parser_replace.c -o parser_rep
fi

# Автоматическая конвертация .xlsx в .csv
# Ищем все файлы .xlsx в текущей директории
xlsx_files=(*.xlsx)

# Проверяем, существуют ли xlsx файлы (чтобы не обрабатывать пустую маску *.xlsx)
if [ -e "${xlsx_files[0]}" ]; then
    echo "Найдена таблица(ы) XLSX. Конвертируем в CSV..."
    for xlsx in "${xlsx_files[@]}"; do
        # Формируем имя для нового csv-файла (заменяем расширение)
        csv_name="${xlsx%.xlsx}.csv"
        
        # Конвертируем с разделителем точка с запятой (-d ';')
        xlsx2csv -d ';' "$xlsx" "$csv_name"
        echo "Сконвертировано: $xlsx -> $csv_name"
    done
fi

# Инициализируем два пустых массива для файлов
cm_files=()
regular_files=()

# 2. Перебираем все CSV файлы в текущей папке
for file in *.csv; do
    # Пропускаем файлы результатов и проверяем, что файл вообще существует
    [[ "$file" == *"_output.csv" ]] && continue
    [[ ! -f "$file" ]] && continue

    # Проверяем, начинается ли файл на цм/цм/cm/cm (регистронезависимо)
    if [[ "$file" =~ ^([Cc][Mm]|[Цц][Мм]) ]]; then
        cm_files+=("$file")
    else
        regular_files+=("$file")
    fi
done

# 3. Запуск для файлов с префиксом ЦМ/CM
if [ ${#cm_files[@]} -gt 0 ]; then
    echo "Обработка файлов ЦМ/CM (с флагом -cm)..."
    ./parser_rep \
        -cm \
        "${cm_files[@]}"
fi

# 4. Запуск для всех остальных файлов
if [ ${#regular_files[@]} -gt 0 ]; then
    echo "Обработка обычных файлов (без флага)..."
    ./parser_rep \
        "${regular_files[@]}"
fi

echo "Все файлы успешно обработаны!"
