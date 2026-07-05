#include <stdio.h>
#include <string.h>

#define BUFF_SIZE 512
#define MAX_BUFF 4096
#define MIN_BUFF 32

#define LINE_NUM_ST_CM 3 // код для возврата из функции

typedef struct {

	int is_cmcrcm;
	
	char stative[MIN_BUFF];
	char polka[MIN_BUFF];
	char place[MIN_BUFF];
	
	char *name_2_line; // Имя устройства
	char *place_3_line; // место установки
	char *type_4_line; // тип устройства
	char *period_5_line; // периодичность
	char *zavod_6_line; // заводской
	char *inventory_7_line; // инвентарный
	char *year_8_line; // год выпуска
	char *date_rep_9_line; // дата замены
	char *date_next_10_line; // дата следующей замены
	char *full_name_11_line; // ФИО
}Table;

static int parsing_line(char *line, Table *table);
static int parse_place(Table *table);
static int formated_period(char *period, const char *table_period);

int main(int argc, char *argv[])
{
	if(argc < 2)
		return 1;

	Table table;

	int start;
	
	if(strncmp(argv[1], "-cm", 3) == 0){
		table.is_cmcrcm = 1;
		start = 2;
	} else {
		table.is_cmcrcm = 0;
		start = 1;	
	} 

	
	for(int i = start; i < argc; i++)
	{
		const char *input_name = argv[i];
		char output_name[BUFF_SIZE];
		snprintf(output_name, BUFF_SIZE, "%s_output.csv", input_name);
	
		FILE *input = fopen(input_name, "rb");
		if(!input){
			printf("invalid input file\n");
			return 1;
		}
	
		FILE *output = fopen(output_name, "wb");
		if(!output){
			printf("invalid output file\n");
			return 1;
		}
		
		char buf[MAX_BUFF];
		char new_line[MAX_BUFF];

		int found = 0;
		// убираем мусор
		while(fgets(buf, sizeof(buf), input) != NULL){
			if(strncmp(buf, "1;2;3;4;", 8) == 0){
				found = 1;
				break;	
			}
		}

		if(!found){
			printf("Не нашли строку где заканчиваются мусорные данные\n");
			return 1;
		}
	
		while(fgets(buf, sizeof(buf), input) != NULL){
			if(strncmp(buf, ";;;;", 4) != 0){
			//	printf("\n\t[%d] PARSE RESULT\t\n", k);
			    if(parsing_line(buf, &table) == LINE_NUM_ST_CM){
			    	continue;
			    }
				parse_place(&table);
				// формируем строку периодичности
				char period[BUFF_SIZE];
				formated_period(period, table.period_5_line);
				
				snprintf(new_line,MAX_BUFF,
					"=\"%s\";"
					"=\"%s\";"
					"=\"%s\";"
					"=\"%s\";"
					"=\"%s\";"
					"=\"%s\";"// Периодичность
					"=\"%s\";"
					"=\"%s\";"
					"=\"%s\";"
					"=\"%s\";"
					"\r\n",
					table.stative,
					table.polka,
					table.place,
					table.type_4_line,
					table.name_2_line,
					period,
					table.date_rep_9_line,
					table.date_next_10_line,
					table.inventory_7_line,
					table.zavod_6_line
				);
				fputs(new_line, output);

			}
		}
		
		fclose(input);
		fclose(output);	
	}
	
	return 0;
}

static int ConvertInt(const char *table_period){
	int sum = 0;
	char const *ptr = table_period;
	while(*ptr && (*ptr >= '0' && *ptr <= '9')){
		sum = 10 * sum + (*ptr - '0');
		ptr++;
	}
	return sum;
}

static int formated_period(char *period, const char *table_period){
	
	int num = ConvertInt(table_period);
	
	if(num == 0){
		*period = '\0';
		return 0;
	}
	if (num % 100 >= 11 && num % 100 <= 14) {
		snprintf(period, BUFF_SIZE, "Раз в %s лет", table_period);
		return 0;
	}

	switch (num % 10) {
	        case 1:  snprintf(period, BUFF_SIZE, "Раз в %s год", table_period); // 1 год, 21 год
	        		 break;  
	        case 2:
	        case 3:
	        case 4: snprintf(period, BUFF_SIZE, "Раз в %s года", table_period); // 2 года, 34 года
	        		break; 
	        default: snprintf(period, BUFF_SIZE, "Раз в %s лет", table_period);
	    }
	
	return 0;	
}

static int parsing_line(char *line, Table *table){
	
	char *ptr = line;
	
	int number_column = 2;
	int i = 0;

	if(table->is_cmcrcm && *ptr != ';'){
		while(*ptr){
			if(*ptr < '0' || *ptr > '9'){
				break;
			}
			table->stative[i] = *ptr;
			i++; 	
			ptr++;
		}
		return LINE_NUM_ST_CM;
	}
	
	while(*ptr && *ptr != '\r' && *ptr != '\n'){
		if(*ptr == ';' || *ptr == '\''){

			memset(table->polka, 0, sizeof(table->polka));
			memset(table->place, 0, sizeof(table->place));
			
			*ptr = '\0';

			switch(number_column){
				case 2: table->name_2_line = ++ptr; break;
				case 3: table->place_3_line = ++ptr; break;
				case 4: table->type_4_line = ++ptr; break;
				case 5: table->period_5_line = ++ptr; break;
				case 6: table->zavod_6_line = ++ptr; break;
				case 7: table->inventory_7_line = ++ptr; break;
				case 8: table->year_8_line = ++ptr; break;
				case 9: table->date_rep_9_line = ++ptr; break;
				case 10: table->date_next_10_line = ++ptr; break;
				case 11: table->full_name_11_line = ++ptr; break;
				default: break;
					
			}
			number_column++;
			continue;
		} 
			
		ptr++;
	}
	
	return 0;
}

static int parse_place(Table *table){

	if(table->place_3_line == NULL) return 0;
	char *ptr = table->place_3_line;
	int i = 0;

	// удаляем лишние пробелы в начале если есть
	while(*ptr == ' ') ptr++;
		
	if(!table->is_cmcrcm){
		while(*ptr){
			if(*ptr == '-'){
				*ptr++ = '\0';
				break;
			}
			table->stative[i] = *ptr;
			i++;
			ptr++;
		}
		
	} else {
		// для цмкрцм
		if(*ptr < '0' || *ptr > '9'){
			snprintf(table->place, sizeof(table->place), "%s", table->place_3_line);
			return 0;
		}
	
	}

	char *tmp_ptr = ptr;
	char *tmp = ptr;
	
	table->polka[0] = *ptr++;
	table->polka[1] = *ptr++;
	table->polka[2] = '\0';
	
	if(!*ptr)
		return 0;

	int found = 0;

	// ищем буквы, если они есть место установки будет полным
	if(table->is_cmcrcm){
		while(*tmp_ptr){
			if((*tmp_ptr < '0' || *tmp_ptr > '9') && *tmp_ptr != ' ' && *tmp_ptr != '-'){
				found = 1;
				break;	
			} 
			tmp_ptr++;
		}
	}
	
	while(*ptr == '-' || *ptr == ' ') ptr++;

	if(found)
		ptr = tmp;

	i = 0;
	
	while(*ptr){
		table->place[i++] = *ptr;
		ptr++;
	}	
		
	return 0;		
}
