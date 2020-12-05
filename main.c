#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<pthread.h>
#include 	<allegro.h>
#include 	<math.h>
#include 	<stdbool.h>
#include 	"task.h"


/********************* MACRO ***************************/


// TASKS IDENTIFIER MACRO

#define		GRAPHIC_TASK		0
#define 	USER_TASK		1
#define 	MOVE_TASK		2

// TASKS PERIOD MACRO

#define		GRAPHIC_PERIOD		300
#define		USER_PERIOD		50
#define	 	MOVE_PERIOD		50


// MACRO PER LA GRAFICA

#define 	DIM_TEXT		48		//dimensione del testo 
#define		DIM_VALUE_X		180		//dimensione dei valori da visualizzare
#define 	BOX			4		//numero di righe e colonne
#define 	SPACE			120		//spazio tra due righe/colonne


// MACRO PER IL GIOCO

#define		CONST			33		//costante da salvare nella profile_matrix
#define		INTERV			4



/****************** GLOBAL VARIABLES *******************/

//	variabili tastiera

bool		quit			=	false;		//  variabile di terminazione 
bool		begin			=	false;		//  variabile di inizio gioco
bool		left			= 	false;		//  variabile per muovere a sinistra
bool		right			= 	false;		//  variabile per muovere a destra
bool		up			= 	false;		//  variabile per muovere verso l'alto
bool		down			= 	false;		//  variabile per muovere verso sinistra


//	variabili gioco
	
int 		matrix[BOX][BOX]; 			// matrice dinamica in cui vengono caricati i valori
int		profile_matrix[BOX][BOX];		// matrice utile nei calcoli e negli spostamenti
int 		temp_matrix[BOX][BOX];			// matrice temporanea

//	mutex

pthread_mutex_t 	mutex 		= 	PTHREAD_MUTEX_INITIALIZER;



//	gruppo di variabili per la grafica

int		x = 1024, y = 720, col = 4;
int		rect_coord_x1 = 160; 			// 	x / 4
int		rect_coord_x2 = 640; 			// 	3/4 * x 
int		rect_coord_y1 = 480;
int		rect_coord_y2 = 120;
int		num_position_x = 225;			//	posizione dei numeri nella tabella
int		num_position_y = 170;



/****************** TASK PROTOTYPES ********************/

void		*graphic_task();			//	task per la grafica
void		*user_command();			//	task dedito alla lettura dei comandi dell'utente
void 		*move_task();				// 	task per muovere/sommare i numeri della matrice


/**************** FUNCTION PROTOTYPES ******************/

bool		init();					//	funzione di inizializzazione di dati, variabili e grafica
bool 		crea_matrice();				//	funzione che crea la matrice iniziale
int		draw_rect();				//	funzione per il disegno del rettangolo del grafico
void		read_command(char key);			//	interprete dei comandi inseriti dall'utente
void 		reset_graphic();			//	resetta la grafica ogni Ts
void 		display_numbers();			//	visualizza a display la matrice
void		move_left();				// 	funzione per muovere la matrice a sinistra
void		move_right();				// 	funzione per muovere la matrice a destra
void		move_up();				// 	funzione per muovere la matrice verso l'alto
void		move_down();				// 	funzione per muovere la matrice verso il basso
int 	 *	spread_algorithm(int spread[]);

/******************************* MAIN FUNCTION ********************************/


int main(void)
{

	if (init() != true)
		return 1;
		

	// tasks creation

	task_create(graphic_task,	GRAPHIC_TASK,	    GRAPHIC_PERIOD,	1000,	20);
	task_create(user_command,	USER_TASK,	    USER_PERIOD,	500,	20);
	task_create(move_task,		MOVE_TASK,	    MOVE_PERIOD,	500,	20);
	

	// tasks joining

	pthread_join(tid[GRAPHIC_TASK],NULL);
	printf("GRAPHIC TASK\n");
	pthread_join(tid[USER_TASK],NULL);
	printf("USER TASK\n");
	pthread_join(tid[MOVE_TASK],NULL);
	printf("MOVE TASK\n");

	

	return 0;
}



/*************************** TASK IMPLEMENTATION ******************************/


//---------------------------------------------------------------------------
//                  Task per l'aggiornamento grafico          
//---------------------------------------------------------------------------


void *graphic_task()
{

	set_activation(GRAPHIC_TASK);

	while (quit == false) {
		
		do {
			continue;
			

		} while (begin == false);
		
		pthread_mutex_lock(&mutex);
		display_numbers();
		pthread_mutex_unlock(&mutex);
	
		if (deadline_miss(GRAPHIC_TASK) == 1) //printf("DEADLINE MISS GRAPHIC\n");     //soft real time
		wait_for_activation(GRAPHIC_TASK);
	}	
	return NULL;
}


//---------------------------------------------------------------------------
//            Task di lettura ed interpretazione comandi utente           
//---------------------------------------------------------------------------


void *user_command()
{

char	key;				//Salviamo qui il carattere inserito dall'utente

    set_activation(USER_TASK);

    while (quit == false) {

		if(keypressed()){
        	key = readkey() & 0xFF;
        	read_command(key);
		}

		
        if (deadline_miss(USER_TASK) == 1) printf("DEADLINE MISS USER COMMAND\n");     //soft real time
        wait_for_activation(USER_TASK);
    }
}


//---------------------------------------------------------------------------
//                                 Task gioco          
//---------------------------------------------------------------------------


void *move_task()
{


    set_activation(MOVE_TASK);

    while (quit == false) {


	if (left == true) {
		move_left();
		left == false;
	}

/*	if (right == true) {
		move_right();
		right == false;
	}
*/
	if (up == true) {
		move_up();
		
	}

	if (down == true) {
		move_down();
		down == false;
	}

	
/*	reset_graphic();
	display_numbers();*/
	
		
      //  if (deadline_miss(MOVE_TASK) == 1) printf("DEADLINE MISS MOVE TASK\n");     //soft real time
        wait_for_activation(USER_TASK);
    }
}



/*************************** FUNCTIONS IMPLEMENTATION ********************************/


//--------------------------------------------------------------------------
//            		 Inizializzazione Allegro 
//--------------------------------------------------------------------------

bool init()
{

char 	text[DIM_TEXT];
char 	value_x[DIM_VALUE_X];
	
	srand(time(NULL));

	crea_matrice();
   	if (allegro_init() != 0)
		return false;

    	install_keyboard();
	install_mouse();
    	set_color_depth(8);
    	set_gfx_mode(GFX_AUTODETECT_WINDOWED, x, y, 0, 0); 
    	enable_hardware_cursor();
    	show_mouse(screen);
	clear_to_color(screen, makecol(0, 0, 0));

	draw_rect();	
	
	sprintf(value_x, "Press 'q' to exit");
	textout_centre_ex(screen, font, value_x, rect_coord_x1 + 70, rect_coord_y1 - 400, 15, -1);

	sprintf(value_x, "Press 's' to start the game");
	textout_centre_ex(screen, font, value_x, rect_coord_x1 + 100, rect_coord_y1 - 450, 15, -1);
	


	return true;
}


//---------------------------------------------------------------------------
//             Funzione per l'inizializzazione della grafica
//---------------------------------------------------------------------------


int draw_rect()
{

int 	i = 0;

	rectfill(screen, rect_coord_x1, rect_coord_y1 + SPACE, rect_coord_x2, rect_coord_y2, 15);
	rect(screen, rect_coord_x1, rect_coord_y1 + SPACE, rect_coord_x2, rect_coord_y2, 4);
	
	// linee verticali
	for (i = 0; i < BOX + 1; i++) { 
		line(screen, rect_coord_x1 + i * SPACE, rect_coord_y1 + SPACE, rect_coord_x1 + i * SPACE, rect_coord_y2, 9); // 11
		line(screen, rect_coord_x1 + i * SPACE -1, rect_coord_y1 + SPACE, rect_coord_x1 + i * SPACE -1, rect_coord_y2, 9); // 11
		line(screen, rect_coord_x1 + i * SPACE +1, rect_coord_y1 + SPACE, rect_coord_x1 + i * SPACE +1, rect_coord_y2, 9); // 11
	}

	// linee orizzontali	
	for (i = 0; i < BOX + 1; i++) { 
		line(screen, rect_coord_x1, rect_coord_y2 + i * SPACE, rect_coord_x2, rect_coord_y2 + i * SPACE, 9);
		line(screen, rect_coord_x1, rect_coord_y2 + i * SPACE -1, rect_coord_x2, rect_coord_y2 + i * SPACE -1, 9);
		line(screen, rect_coord_x1, rect_coord_y2 + i * SPACE +1, rect_coord_x2, rect_coord_y2 + i * SPACE +1, 9);
	}

	return 0;
}


//---------------------------------------------------------------------------
//          Funzione per la lettura dei comandi da tastiera
//---------------------------------------------------------------------------


void read_command(char key)
{
	
	switch (key) {

		case 'q':
			quit = true;
			break;


		case 's':
			begin = true;
			break;


		case 'j':
			left = true;
			break;


		case 'l':
			right = true;
			break;


		case 'i':
			up = true;
			break;


		case 'k':
			down = true;
			break;

	}
	
	return;
}


//---------------------------------------------------------------------------
//                    Inizializza la matrice di gioco
//---------------------------------------------------------------------------


bool crea_matrice()
{
	
int 	line_1, line_2;
int 	column_1, column_2;
int 	i = 0, j = 0;

	
	for (i = 0; i < BOX; i++) {
		for (j = 0; j < BOX; j++) {
			matrix[i][j] = 0;
		}
	}
	
	line_1 		=	 rand() % (BOX);
	column_1	= 	 rand() % (BOX);
	line_2 		=	 rand() % (BOX);
	column_2	= 	 rand() % (BOX);


	//printf("linea_1 = %d, colonna_1 = %d\n", line_1+1, column_1+1);
	//printf("linea_2 = %d, colonna_2 = %d\n", line_2+1, column_2+1);

	matrix[line_1][column_1] = 2;
	matrix[line_2][column_2] = 2;
	

	for (i = 0; i < BOX; i++) {
		
		for (j = 0; j < BOX; j++) {
			printf("%d	", matrix[i][j]);
		}
	printf("\n");
	}

	//setto la profile_matrix
	for (i = 0; i < BOX; i++) {
		
		for (j = 0; j < BOX; j++) {
			profile_matrix[i][j] = 0;
		}
	}
	
	
}


//---------------------------------------------------------------------------
//                           Resetta la grafica
//---------------------------------------------------------------------------


void reset_graphic()
{

int 	i  =  0, j  =  0;


	rectfill(screen, rect_coord_x1 - 30, rect_coord_y1 - 430, rect_coord_x1 + 230, rect_coord_y1 - 465, 0); 
	

	for (i = 0; i < BOX; i++) {
							//resetto tutti i 16 quadrati
		for (j = 0; j < BOX; j++){												
			rectfill(screen, rect_coord_x1 + 10 + i*SPACE, rect_coord_y1 - 350 + j*SPACE, rect_coord_x1 + 110 + i*SPACE, rect_coord_y1 - 250 + j*SPACE, 15); 
		}

	}
}


//---------------------------------------------------------------------------
//                    Visualizzazione dinamica matrice
//---------------------------------------------------------------------------


void display_numbers()
{

char 	value_x[DIM_VALUE_X];
int 	i = 0, j = 0;


	for (i = 0; i < BOX; i++) {
		for (j = 0; j < BOX; j++) {

			if (matrix[i][j] == 0) {
				continue;
			}
			
			else { 
				sprintf(value_x, "%d",  matrix[i][j]);
				textout_centre_ex(screen, font, value_x, num_position_x + (j*SPACE), num_position_y + (i*SPACE), 10, -1);
			}
		}
	}


}


//---------------------------------------------------------------------------
//                       Muove tutto
//---------------------------------------------------------------------------


int * spread_algorithm(int spread[])
{

int 	i;
bool 	done = false;
int 	incr = 0;

	do {
		incr = 0;
		done = true;
		for (i = 0; i < BOX*BOX - BOX; i++) {

			if (spread[i] != 0 && spread[i] == spread[i+INTERV]) {
				spread[i]	  = 	spread[i] + spread[i+INTERV];
				spread[i+INTERV]  = 	0;
				done = false;
			}
			else if (spread[i] == 0 && spread[i+INTERV] != 0) {
				spread[i] = spread[i+INTERV];
				spread[i+INTERV] = 0;
				done = false;
			}
		}		

	} while (done == false);


	return spread;
}

//---------------------------------------------------------------------------
//                         Muove tutto a sinistra
//---------------------------------------------------------------------------


void move_left()
{

int 	i, j, ind = 0; 
int 	spread[BOX*BOX];
int  *  sp;

	pthread_mutex_lock(&mutex);
	printf("Move left!\n");
 
	for(int i = 0; i < BOX; i++) {
		for(int j = 0; j < BOX; j++) {
			spread[ind] = matrix[BOX-j-1][i];
			ind++;
		}
	}

	sp = spread_algorithm(spread);

	ind = 0;
	for(int i = 0; i < BOX; i++) {
		for(int j = 0; j < BOX; j++) {
			matrix[BOX-j-1][i] = sp[ind];
			ind++;
		}
	}
		
	pthread_mutex_unlock(&mutex);
	left = false;
	
	reset_graphic();
	display_numbers();


}


//---------------------------------------------------------------------------
//                           Muove tutto a destra
//---------------------------------------------------------------------------


void move_right()
{




}


//---------------------------------------------------------------------------
//                        Muove tutto verso l'alto
//---------------------------------------------------------------------------


void move_up()
{

int 	i, j, ind = 0; 
int 	spread[BOX*BOX];
int  *  sp;

	pthread_mutex_lock(&mutex);
	printf("Move up!\n");
	// spalmo i valori su un unico vettore
 
	for (i = 0; i < BOX; i++) {
		for (j = 0; j < BOX; j++){	
			spread[ind] = matrix[i][j];
			ind++;
		}
	}

	sp = spread_algorithm(spread);

	ind = 0;
	for (i = 0; i < BOX; i++) {
		for (j = 0; j < BOX; j++) {
			matrix[i][j] = sp[ind];
			ind++;
		}
			
	}
	

	pthread_mutex_unlock(&mutex);
	up = false;
	
	reset_graphic();
	display_numbers();

}


//---------------------------------------------------------------------------
//                       Muove tutto verso sinistra
//---------------------------------------------------------------------------


void move_down()
{

int 	i, j, ind = 0; 
int 	temp[BOX*BOX];
int 	spread[BOX*BOX];
int  *  sp;

	pthread_mutex_lock(&mutex);
	printf("Move down!\n");
	for (i = 0; i < BOX; i++) {
		for (j = 0; j < BOX; j++){	
			temp[ind] = matrix[i][j];
			ind++;
		}
	}


	for (i = 0; i < BOX*BOX; i++) {
		spread[i] = temp[BOX*BOX-i-1];
	}


	sp = spread_algorithm(spread);


	for (i = 0; i < BOX*BOX; i++)
		printf("%d", sp[i]);
	printf("\n");

	ind = 0;
	for (i = BOX-1; i > -1; i--) {
		for (j = BOX-1; j > -1; j--) {
			matrix[i][j] = sp[ind];
			ind++;
		}
			
	}

	pthread_mutex_unlock(&mutex);
	down = false;
	
	reset_graphic();
	display_numbers();

}





















