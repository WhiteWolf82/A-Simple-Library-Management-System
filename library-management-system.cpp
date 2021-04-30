#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<mysql.h>
#include<time.h>

#define HOST "localhost"
#define USERNAME "root"
#define PASSWORD "XXXXXX"
#define DATABASE "library"


void get_borrow_date(char date[])	//get borrow/current date
{
	char yy[10], mm[10], dd[10];
	time_t t;
	struct tm* lt;
	time(&t);
	lt = localtime(&t);
	itoa(lt->tm_year + 1900, yy, 10);
	itoa(lt->tm_mon + 1, mm, 10);
	itoa(lt->tm_mday, dd, 10);

	strcat(date, yy);
	strcat(date, "-");
	strcat(date, mm);
	strcat(date, "-");
	strcat(date, dd);
}

void get_return_date(char date[])	//get return date
{
	char yy[10], mm[10], dd[10];
	time_t t;
	struct tm* lt;
	time(&t);

	t += 30 * 60 * 60 * 24;
	lt = localtime(&t);
	itoa(lt->tm_year + 1900, yy, 10);
	itoa(lt->tm_mon + 1, mm, 10);
	itoa(lt->tm_mday, dd, 10);

	strcat(date, yy);
	strcat(date, "-");
	strcat(date, mm);
	strcat(date, "-");
	strcat(date, dd);
}

void show_books(MYSQL conn)
{
	MYSQL_RES *res = NULL;
	MYSQL_ROW row;
	int rows, fields;
	int i, j;
	char query_str[100] = "select * from book";
	int rc = mysql_real_query(&conn, query_str, strlen(query_str));
	if (rc != 0)
	{
		printf("Error!\n");
		return;
	}

	res = mysql_store_result(&conn);
	if (res == NULL)
	{
		printf("Error!\n");
		return;
	}
	
	rows = mysql_num_rows(res);
	fields = mysql_num_fields(res);
	printf("\n");
	printf("book_ID\t\tbook_name\t\tprice\t      number\t\t\n");
	while ((row = mysql_fetch_row(res)))
	{
		for (i = 0; i < fields; i++)
		{
			if (i == 1)
			{
				printf("%.25s", row[i]);
				for (j = 0; j < 25 - strlen(row[i]); j++)
					printf(" ");
			}
			else
				printf("%s\t\t", row[i]);
		}
		printf("\n");
	}
}

void show_cards(MYSQL conn)
{
	MYSQL_RES *res = NULL;
	MYSQL_ROW row;
	int rows, fields;
	int i;
	char query_str[100] = "select * from card";
	int rc = mysql_real_query(&conn, query_str, strlen(query_str));
	if (rc != 0)
	{
		printf("Error!\n");
		return;
	}

	res = mysql_store_result(&conn);
	if (res == NULL)
	{
		printf("Error!\n");
		return;
	}

	rows = mysql_num_rows(res);
	fields = mysql_num_fields(res);
	printf("\n");
	printf("card_ID\t\tuser_name\t\t\n");
	while ((row = mysql_fetch_row(res)))
	{
		for (i = 0; i < fields; i++)
			printf("%s\t\t", row[i]);
		printf("\n");
	}
}

void show_record(MYSQL conn)
{
	MYSQL_RES *res = NULL;
	MYSQL_ROW row;
	int rows, fields;
	int i;
	char query_str[100] = "select * from record";
	int rc = mysql_real_query(&conn, query_str, strlen(query_str));
	if (rc != 0)
	{
		printf("Error!\n");
		return;
	}

	res = mysql_store_result(&conn);
	if (res == NULL)
	{
		printf("Error!\n");
		return;
	}

	rows = mysql_num_rows(res);
	fields = mysql_num_fields(res);
	printf("\n");
	printf("record_ID\t\tcard_ID\t\tbook_ID\t\tborrow_date\t\treturn_date\n");
	while ((row = mysql_fetch_row(res)))
	{
		for (i = 0; i < fields; i++)
			printf("%s\t\t", row[i]);
		printf("\n");
	}
}

void add_book(MYSQL conn)
{
	MYSQL_RES *tmp = NULL;
	MYSQL_ROW row;
	int res;
	int i;
	int flag = 1;
	char query_str[100] = "insert into book values('";
	char tmp_query[100] = "select * from book";
	char book_ID[100], book_name[100], price[100], record_number[100];

	printf("Enter the ID of the book.\n");
	scanf("%s", book_ID);
	printf("Enter the name of the book.\n");
	getchar();
	gets_s(book_name);
	printf("Enter the price of the book.\n");
	scanf("%s", price);
	printf("Enter the number of the books you want to add.\n");
	scanf("%s", record_number);

	int rc = mysql_real_query(&conn, tmp_query, strlen(tmp_query));
	if (rc != 0)
	{
		printf("Error!\n");
		return;
	}
	tmp = mysql_store_result(&conn);
	while ((row = mysql_fetch_row(tmp)))
	{
		if (strcmp(book_ID, row[0]) == 0)
		{
			flag = 0;
			break;
		}
	}

	if (flag == 0)	//book already exists
	{
		if ((strcmp(book_name, row[1]) != 0) || (atof(price) != atof(row[2])))
		{
			printf("Error! Book already exists, but the information doesn't match!\n");
			return;
		}
		char update_str[100] = "update book set record_number = '";
		char new_record[20];
		itoa(atoi(row[3]) + atoi(record_number), new_record, 10);
		strcat(update_str, new_record);
		strcat(update_str, "'where book_ID = '");
		strcat(update_str, row[0]);
		strcat(update_str, "'");
		res = mysql_query(&conn, update_str);
		printf("Book already exists, and its number has been increased.\n");
		return;
	}
	else  //new book
	{
		//get the query
		strcat(query_str, book_ID);
		strcat(query_str, "','");
		strcat(query_str, book_name);
		strcat(query_str, "',");
		strcat(query_str, price);
		strcat(query_str, ",");
		strcat(query_str, record_number);
		strcat(query_str, ")");

		res = mysql_query(&conn, query_str);
		if (!res)
		{
			printf("Inserting successed!\n");
		}
		else
		{
			printf("Inserting failed.\n");
		}
	}
}

void borrow_book(MYSQL conn)
{
	MYSQL_RES *tmp = NULL;
	MYSQL_ROW row;
	int res;
	int i;
	int flag = 1;
	int rc;
	char query_str[100] = "insert into record values('";
	char record_query[100] = "select * from record";
	char book_query[100] = "select * from book";
	char card_query[100] = "select * from card";
	char record_ID[100] = { 0 }, card_ID[100] = { 0 }, book_ID[100] = { 0 };

	printf("Enter the card ID.\n");
	scanf("%s", card_ID);

	int whether_user = 0;
	rc = mysql_real_query(&conn, card_query, strlen(card_query));
	tmp = mysql_store_result(&conn);
	while (row = mysql_fetch_row(tmp))
	{
		if (strcmp(card_ID, row[0]) == 0)
		{
			whether_user = 1;
			break;
		}
	}
	if (whether_user == 0)
	{
		printf("No such user!\n");
		return;
	}

	rc = mysql_real_query(&conn, record_query, strlen(record_query));
	if (rc != 0)
	{
		printf("Error!\n");
		return;
	}
	tmp = mysql_store_result(&conn);
	while (row = mysql_fetch_row(tmp))
	{
		if (strcmp(card_ID, row[1]) == 0)
		{
			flag = 0;
			break;
		}
	}

	if (flag == 0)
	{
		printf("There is a book that the user hasn't returned yet.\n");
		printf("\n");
		printf("record_ID\t\tcard_ID\t\tbook_ID\t\tborrow_date\t\treturn_date\n");
		for (i = 0; i < 5; i++)
			printf("%s\t\t", row[i]);
		printf("\n\n");
		return;
	}
	else
	{
		int op;
		printf("1. Search by book_name\n");
		printf("2. Search by book_ID\n");
		scanf("%d", &op);

		if (op == 1)
		{
			int flag1 = 0;
			char book_name[100];
			printf("Enter the name of the book.\n");
			getchar();
			gets_s(book_name);

			rc = mysql_real_query(&conn, book_query, strlen(book_query));
			if (rc != 0)
			{
				printf("Error!\n");
				return;
			}
			tmp = mysql_store_result(&conn);
			while ((row = mysql_fetch_row(tmp)))
			{
				if (strcmp(book_name, row[1]) == 0)
				{
					flag1 = 1;
					break;
				}
			}
			
			if (flag1 == 0)
			{
				printf("No such book!\n");
				return;
			}
			else
			{
				if (atoi(row[3]) == 0)
				{
					printf("None of this book is available now!\n");
					return;
				}

				char borrow_date[20] = { 0 }, return_date[20] = { 0 };
				get_borrow_date(borrow_date);
				get_return_date(return_date);

				strcpy(book_ID, row[0]);

				//record_ID consists of card_ID and book_ID
				strcat(record_ID, card_ID);
				strcat(record_ID, book_ID);

				strcat(query_str, record_ID);
				strcat(query_str, "','");
				strcat(query_str, card_ID);
				strcat(query_str, "','");
				strcat(query_str, book_ID);
				strcat(query_str, "','");
				strcat(query_str, borrow_date);
				strcat(query_str, "','");
				strcat(query_str, return_date);
				strcat(query_str, "')");

				res = mysql_query(&conn, query_str);
				if (!res)
				{
					char update_str[100] = "update book set record_number = '";
					char new_record[20];
					itoa(atoi(row[3]) - 1, new_record, 10);
					strcat(update_str, new_record);
					strcat(update_str, "'where book_ID = '");
					strcat(update_str, row[0]);
					strcat(update_str, "'");
					mysql_query(&conn, update_str);

					printf("Borrowing successed!\n");
				}
				else
				{
					printf("Error! Borrowing failed.\n");
				}
			}
		}
		else if (op == 2)
		{
			int flag2 = 0;
			printf("Enter the ID of the book.\n");
			scanf("%s", book_ID);

			rc = mysql_real_query(&conn, book_query, strlen(book_query));
			if (rc != 0)
			{
				printf("Error!\n");
				return;
			}
			tmp = mysql_store_result(&conn);
			while ((row = mysql_fetch_row(tmp)))
			{
				if (strcmp(book_ID, row[0]) == 0)
				{
					flag2 = 1;
					break;
				}
			}

			if (flag2 == 0)
			{
				printf("No such book!\n");
				return;
			}
			else
			{
				if (atoi(row[3]) == 0)
				{
					printf("None of this book is available now!\n");
					return;
				}

				char borrow_date[20] = { 0 }, return_date[20] = { 0 };
				get_borrow_date(borrow_date);
				get_return_date(return_date);

				strcpy(book_ID, row[0]);

				//record_ID consists of card_ID and book_ID
				strcat(record_ID, card_ID);
				strcat(record_ID, book_ID);

				strcat(query_str, record_ID);
				strcat(query_str, "','");
				strcat(query_str, card_ID);
				strcat(query_str, "','");
				strcat(query_str, book_ID);
				strcat(query_str, "','");
				strcat(query_str, borrow_date);
				strcat(query_str, "','");
				strcat(query_str, return_date);
				strcat(query_str, "')");

				res = mysql_query(&conn, query_str);
				if (!res)
				{
					char update_str[100] = "update book set record_number = '";
					char new_record[20];
					itoa(atoi(row[3]) - 1, new_record, 10);
					strcat(update_str, new_record);
					strcat(update_str, "'where book_ID = '");
					strcat(update_str, row[0]);
					strcat(update_str, "'");
					mysql_query(&conn, update_str);

					printf("Borrowing successed!\n");
				}
				else
				{
					printf("Error! Borrowing failed.\n");
				}
			}
		}
	}
}

void return_book(MYSQL conn)
{
	MYSQL_RES *tmp = NULL;
	MYSQL_ROW row, book_row;
	int res;
	int i;
	int flag = 0;
	int rc;
	char query_str[100] = "delete from record where record_ID = '";
	char record_query[100] = "select * from record";
	char book_query[100] = "select * from book";
	char card_query[100] = "select * from card";
	char record_ID[100] = { 0 }, card_ID[100] = { 0 }, book_ID[100] = { 0 };

	printf("Enter the card ID.\n");
	scanf("%s", card_ID);

	int whether_user = 0;
	rc = mysql_real_query(&conn, card_query, strlen(card_query));
	tmp = mysql_store_result(&conn);
	while (row = mysql_fetch_row(tmp))
	{
		if (strcmp(card_ID, row[0]) == 0)
		{
			whether_user = 1;
			break;
		}
	}
	if (whether_user == 0)
	{
		printf("No such user!\n");
		return;
	}

	rc = mysql_real_query(&conn, record_query, strlen(record_query));
	if (rc != 0)
	{
		printf("Error!\n");
		return;
	}
	tmp = mysql_store_result(&conn);
	while (row = mysql_fetch_row(tmp))
	{
		if (strcmp(card_ID, row[1]) == 0)
		{
			flag = 1;
			break;
		}
	}

	if (flag == 0)
	{
		printf("This user hasn't borrowed any book!\n");
		return;
	}
	else
	{
		printf("\n");
		printf("record_ID\t\tcard_ID\t\tbook_ID\t\tborrow_date\t\treturn_date\n");
		for (i = 0; i < 5; i++)
			printf("%s\t\t", row[i]);
		printf("\n\n");

		char current_date[100] = { 0 };
		get_borrow_date(current_date);

		if (strcmp(current_date, row[4]) > 0)
		{
			printf("Out of loan period! Please ask the user to pay fines.\n");
		}
		printf("Renew or not? 1. Y	2. N\n");
		int op;
		scanf("%d", &op);
		if (op == 1)
		{
			char renew_str[100] = "update record set borrow_date = '";
			char borrow_date[100] = { 0 };
			char return_date[100] = { 0 };
			get_borrow_date(borrow_date);
			get_return_date(return_date);

			strcat(renew_str, borrow_date);
			strcat(renew_str, "', return_date = '");
			strcat(renew_str, return_date);
			strcat(renew_str, "' where record_ID = '");
			strcat(renew_str, row[0]);
			strcat(renew_str, "'");
			rc = mysql_query(&conn, renew_str);
			if (rc != 0)
			{
				printf("Renewing failed!\n");
				return;
			}
			else
				printf("Renewing succeed!\n");
		}
		else
		{
			char update_str[100] = "update book set record_number = '";
			char new_record[20];

			mysql_real_query(&conn, book_query, strlen(book_query));
			tmp = mysql_store_result(&conn);
			while (book_row = mysql_fetch_row(tmp))
			{
				if (strcmp(book_row[0], row[2]) == 0)
					break;
			}

			itoa(atoi(book_row[3]) + 1, new_record, 10);
			strcat(update_str, new_record);
			strcat(update_str, "'where book_ID = '");
			strcat(update_str, row[2]);
			strcat(update_str, "'");
			mysql_query(&conn, update_str);

			strcat(query_str, row[0]);
			strcat(query_str, "'");
			rc = mysql_query(&conn, query_str);

			if (rc != 0)
			{
				printf("Returning failed!\n");
				return;
			}
			else
				printf("Returning succeed!\n");
		}
	}
}

void add_card(MYSQL conn)
{
	MYSQL_RES *tmp = NULL;
	MYSQL_ROW row;
	int res;
	int i;
	int flag = 1;
	char query_str[100] = "insert into card values('";
	char tmp_query[100] = "select card_ID from card";
	char card_ID[100], user_name[100];

	printf("Enter the ID of the card.\n");
	scanf("%s", card_ID);
	printf("Enter the name of the user.\n");
	getchar();
	gets_s(user_name);

	int rc = mysql_real_query(&conn, tmp_query, strlen(tmp_query));
	if (rc != 0)
	{
		printf("Error!\n");
		return;
	}
	tmp = mysql_store_result(&conn);
	while ((row = mysql_fetch_row(tmp)))
	{
		if (strcmp(card_ID, row[0]) == 0)
		{
			flag = 0;
			break;
		}
	}

	if (flag == 0)	//card already exists
	{
		printf("Card already exists!\n");
		return;
	}
	else  //new card
	{
		//get the query
		strcat(query_str, card_ID);
		strcat(query_str, "','");
		strcat(query_str, user_name);
		strcat(query_str, "')");

		res = mysql_query(&conn, query_str);
		if (!res)
		{
			printf("Inserting successed!\n");
		}
		else
		{
			printf("Inserting failed.\n");
		}
	}
}

void delete_card(MYSQL conn)
{
	MYSQL_RES *tmp = NULL;
	MYSQL_ROW row;
	int res;
	int i;
	int flag = 0;
	char tmp_query[100] = "select * from card";
	char query_str[100] = "delete from card where card_ID = '";
	char card_ID[100], user_name[100];
	int op;

	printf("1. By card_ID\n");
	printf("2. By user_name\n");
	scanf("%d", &op);

	if (op == 1)
	{
		printf("Enter the ID of the card.\n");
		scanf("%s", card_ID);

		int rc = mysql_real_query(&conn, tmp_query, strlen(tmp_query));
		if (rc != 0)
		{
			printf("Error!\n");
			return;
		}
		tmp = mysql_store_result(&conn);
		while ((row = mysql_fetch_row(tmp)))
		{
			if (strcmp(card_ID, row[0]) == 0)
			{
				flag = 1;
				break;
			}
		}

		if (flag == 0)	//card doesn't exist
		{
			printf("Card doesn't exist!\n");
			return;
		}
		else 
		{
			//get the query
			strcat(query_str, card_ID);
			strcat(query_str, "'");

			res = mysql_query(&conn, query_str);
			if (!res)
			{
				printf("Deleting successed!\n");
			}
			else
			{
				printf("Deleting failed.\n");
			}
		}
	}
	else
	{
		printf("Enter the name of the user.\n");
		scanf("%s", user_name);

		int rc = mysql_real_query(&conn, tmp_query, strlen(tmp_query));
		if (rc != 0)
		{
			printf("Error!\n");
			return;
		}
		tmp = mysql_store_result(&conn);
		while ((row = mysql_fetch_row(tmp)))
		{
			if (strcmp(user_name, row[1]) == 0)
			{
				flag = 1;
				break;
			}
		}

		if (flag == 0)	//card doesn't exist
		{
			printf("Card doesn't exist!\n");
			return;
		}
		else
		{
			printf("\n");
			printf("card_ID\t\tuser_name\t\t\n");

			//make row back to the start
			rc = mysql_real_query(&conn, tmp_query, strlen(tmp_query));
			tmp = mysql_store_result(&conn);
			while ((row = mysql_fetch_row(tmp)))	//if there is more than one records
			{
				if (strcmp(user_name, row[1]) == 0)
				{
					for (i = 0; i < 2; i++)
						printf("%s\t\t", row[i]);
					printf("\n");
				}
				
			}
			printf("Enter the ID of the card that you want to delete.\n");
			scanf("%s", card_ID);

			strcat(query_str, card_ID);
			strcat(query_str, "'");

			res = mysql_query(&conn, query_str);
			if (!res)
			{
				printf("Deleting successed!\n");
			}
			else
			{
				printf("Deleting failed.\n");
			}
		}
	}
}

int main()
{
	MYSQL conn;
	int op;

	mysql_init(&conn);

	if (mysql_real_connect(&conn, HOST, USERNAME, PASSWORD, DATABASE, 3306, NULL, 0))
	{
		printf("Connecting successed!\n");

		printf("Choose the operation you want to take.\n");
		printf("1. Show all books.\n");
		printf("2. Show all ID cards.\n");
		printf("3. Show all on-borrow records.\n");
		printf("4. Add a new book.\n");
		printf("5. Add a new ID card.\n");
		printf("6. Delete an ID card.\n");
		printf("7. Borrow a book.\n");
		printf("8. Return a book.\n");
		printf("0. Exit.\n");

		scanf("%d", &op);

		while (op != 0)
		{
			switch (op)
			{
			case 1: show_books(conn);
				printf("\n");
				break;
			case 2: show_cards(conn);
				printf("\n");
				break;
			case 3: show_record(conn);
				printf("\n");
				break;
			case 4: add_book(conn);
				printf("\n");
				break;
			case 5: add_card(conn);
				printf("\n");
				break;
			case 6: delete_card(conn);
				printf("\n");
				break;
			case 7: borrow_book(conn);
				printf("\n");
				break;
			case 8: return_book(conn);
				printf("\n");
				break;
			default: printf("Invalid input!\n");
				break;
			}

			printf("Choose the operation you want to take.\n");
			printf("1. Show all books.\n");
			printf("2. Show all ID cards.\n");
			printf("3. Show all on-borrow records.\n");
			printf("4. Add a new book.\n");
			printf("5. Add a new ID card.\n");
			printf("6. Delete an ID card.\n");
			printf("7. Borrow a book.\n");
			printf("8. Return a book.\n");
			printf("0. Exit.\n");

			scanf("%d", &op);
		}

		mysql_close(&conn);

	}
	else
		printf("Connecting failed!\n");

	system("pause");
	return 0;
}
