#include <SDL2/SDL.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#define CELL_SIZE 10
#define GRID_ROWS 80
#define GRID_COLS 100


// Couleurs
const SDL_Color COLOR_BG = {255, 255, 255, 255};
const SDL_Color COLOR_GRID = {200, 200, 200, 50};
const SDL_Color COLOR_GRID_INGAME = {225, 225, 225, 50};
const SDL_Color COLOR_CELL = {255, 255, 255, 255};
const SDL_Color COLOR_CELL_HOVER = {0, 0, 0, 255};

// --------------------------------------------------------------------------------------------------------------------- //
// --------------------------------------------- Declaration des variables --------------------------------------------- //
// --------------------------------------------------------------------------------------------------------------------- //

// Dimensions de l'ecran
int SCREEN_WIDTH = GRID_COLS * CELL_SIZE;
int SCREEN_HEIGHT = GRID_ROWS * CELL_SIZE;

int pause = 1;
int wait = 100;
int i,j;
int indice=0;
int generation = 0;



// ---------------------------------------------------------------------------------------------------------------------- //
// -------------------------------------------- Initialisation des fonctions -------------------------------------------- //
// ---------------------------------------------------------------------------------------------------------------------- //

// ----------------------------- Fonction afficher les instructions ----------------------------- //
void instructions(int gen, int pause, int waitTime)
{
    // clear la console
    system("cls");

    // Affichage
    printf("GAME OF LIFE\n\n");
    printf("RULES : \n");
    printf("- Any living cell with fewer than two living neighbours dies, as if by underpopulation.\n");
    printf("- Any living cell with two or three living neighbours lives on to the next generation.\n");
    printf("- Any living cell with more than three living neighbours dies, as if by overpopulation.\n");
    printf("- Any dead cell with exactly three living neighbours becomes a living cell, as if by reproduction.\n\n");
    printf("The sides of the grid are linked to allow more possibilities.\n\n");

    if (pause == 1)
    {
        printf("Click on the white cells to make them live and on the black cells to kill them.\n");
        printf("Press R to get a random pattern.\n");
        printf("Press P to create a row of gliders.\n");
        printf("Press BACKSPACE to empty the grid.\n");
        printf("Press SPACE to play.\n");
        printf("Press ESCAPE to quit.\n");
    } else {
        printf("The game have to be paused to allow any changes to the grid.\n\n");
        printf("Press SPACE to pause the game.\n");
        printf("Press RIGHT or LEFT to change the simulation speed.");
    }

    if (gen != 0)
    {
        printf("\n\nGeneration : %d", gen);
    }

    if (waitTime ==10 && pause == 0)
    {
        printf("\n\nSpeed cannot go up.");
    } else if (waitTime == 1000 && pause == 0)
    {
        printf("\n\nSpeed cannot go down.");
    }
    
    
}

// ----------------------------- Fonction verifier les cellules adjacentes ----------------------------- //
int verify(int** grid, int i, int j)
{
    int count = 0;
    for (int k = -1; k <= 1; k++) {
        for (int l = -1; l <= 1; l++) {
            if (k == 0 && l == 0) continue;
            int row = (i + k + GRID_ROWS) % GRID_ROWS;
            int col = (j + l + GRID_COLS) % GRID_COLS;
            if (grid[row][col] == 1) count++;
        }
    }
    int result = grid[i][j];
    if (grid[i][j] == 1 && (count < 2 || count > 3)) {
        result = 0;
    } else if (grid[i][j] == 0 && count == 3) {
        result = 1;
    }
    return result;
}

// ----------------------------- Fonction afficher la grille de fond ----------------------------- //
void showGrid(SDL_Renderer* renderer)
{
    for (int k = 0; k <= GRID_ROWS; k++) {
        SDL_RenderDrawLine(renderer, 0, k*CELL_SIZE, SCREEN_WIDTH, k*CELL_SIZE);
    }
    for (int k = 0; k <= GRID_COLS; k++) {
        SDL_RenderDrawLine(renderer, k*CELL_SIZE, 0, k*CELL_SIZE, SCREEN_HEIGHT);
    }
    SDL_RenderPresent(renderer);
}

// ----------------------------- Fonction modifier la couleur des cellules ----------------------------- //
void colorCell(int** grid, SDL_Renderer* renderer, int i, int j)
{
    SDL_Rect cellRect = {j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE};
    if (grid[i][j] == 1) {
        SDL_SetRenderDrawColor(renderer, COLOR_CELL_HOVER.r, COLOR_CELL_HOVER.g, COLOR_CELL_HOVER.b, COLOR_CELL_HOVER.a);
    } else {
        SDL_SetRenderDrawColor(renderer, COLOR_CELL.r, COLOR_CELL.g, COLOR_CELL.b, COLOR_CELL.a);
    }
    SDL_RenderFillRect(renderer, &cellRect);
}

// ----------------------------- Fonction initialisation de la mémoire ----------------------------- //
void initializeMemory(int** grid)
{
    for (int i = 0; i < GRID_ROWS; i++) {
        grid[i] = malloc(GRID_COLS * sizeof(int));
    }
    for (int i = 0; i < GRID_ROWS; i++) {
        for (int j = 0; j < GRID_COLS; j++) {
            grid[i][j] = 0;
        }
    }
}

// ----------------------------- Fonction vider la mémoire ----------------------------- //
void freeMemory(int** grid)
{
    for (int i = 0; i < GRID_COLS; i++) {
        free(grid[i]);
    }
    free(grid);
}

// ----------------------------- Fonction creer planeurs ----------------------------- //
void createGliders(int** grid)
{
    for (i = 1; i < GRID_ROWS+20; i+=5)
    {
        grid[40][i] = 1;
        grid[40][i+1] = 1;
        grid[40][i-1] = 1;
        grid[41][i-1] = 1;
        grid[42][i] = 1;
    }
}

// ---------------------------------------------------------------------------------------------------------------------- //
// -------------------------------------------------------- MAIN -------------------------------------------------------- //
// ---------------------------------------------------------------------------------------------------------------------- //
int main( int argc, char* args[] )
{
    // Initialiser SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return 1;
    }

    
    // Creer la fenêtre
    SDL_Window* window = SDL_CreateWindow( "Jeu de la vie", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( window == NULL )
    {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return 1;
    }

    // Creer un renderer pour la fenêtre
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    // Initialiser les grilles et leur memoires
    int **grid = malloc(GRID_ROWS * sizeof(int *));
    initializeMemory(grid);

    int **grid2 = malloc(GRID_ROWS * sizeof(int *));
    initializeMemory(grid2);

    // Mettre la couleur de fond
    SDL_SetRenderDrawColor(renderer, COLOR_BG.r, COLOR_BG.g, COLOR_BG.b, COLOR_BG.a);
    SDL_RenderClear(renderer);

    // Dessiner les lignes de la grille
    SDL_SetRenderDrawColor(renderer, COLOR_GRID.r, COLOR_GRID.g, COLOR_GRID.b, COLOR_GRID.a);
    showGrid(renderer);
    SDL_RenderPresent(renderer);

    
    SDL_Event e;
    int running = 1;
    
    instructions(generation, pause, wait);

    // ----------------------------- Jeu en cours ----------------------------- //
    while (running) {
        while (SDL_PollEvent(&e)) {

            // Attendre la fermeture de la fenêtre
            if (e.type == SDL_QUIT) {
                running = 0;
            }

            // ----------------------------- Pause ----------------------------- //
            if(pause == 1)
            {
                // Enable le clic
                SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_ENABLE);

                // Verification des actions de l'utilisateur
                switch (e.type)
                {
                    // ----------------------------- Action du clavier ----------------------------- //
                    case SDL_KEYDOWN:

                        // Play
                        if (e.key.keysym.sym == SDLK_SPACE) {
                            SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_DISABLE);
                            pause = 0;
                            break;
                        }

                        //Vider la grille
                        if (e.key.keysym.sym == SDLK_BACKSPACE) {
                            for ( i = 0; i < GRID_ROWS; i++)
                            {
                                for (j = 0; j < GRID_COLS; j++)
                                {
                                    grid[i][j]=0;
                                }
                            }
                            generation = 0;
                        }

                        //Grille aleatoire
                        if (e.key.keysym.sym == SDLK_r) {
                            srand(time(NULL));
                            for ( i = 0; i < GRID_ROWS; i++)
                            {
                                for (j = 0; j < GRID_COLS; j++)
                                {
                                    grid[i][j]=rand() % 2;
                                }
                            }
                            generation = 0;
                        }

                        // ligne de Planeur
                        if (e.key.keysym.sym == SDLK_p) {
                            createGliders(grid);
                            generation = 0;
                        }

                        // Quitter
                        if (e.key.keysym.sym == SDLK_ESCAPE) {
                            running = 0;
                            
                        }


                        // Mise à jour des couleurs des cellules
                        for (i = 0; i < GRID_ROWS; i++) {
                            for (j = 0; j < GRID_COLS; j++)
                            {
                                colorCell(grid, renderer, i, j);
                            }
                        }

                        // La grille reste apres la modification
                        SDL_SetRenderDrawColor(renderer, COLOR_GRID.r, COLOR_GRID.g, COLOR_GRID.b, COLOR_GRID.a);
                        showGrid(renderer);

                        // Afficher les modifications
                        SDL_RenderPresent(renderer);




                        break;

                    // ----------------------------- Action de la souris ----------------------------- //
                    case SDL_MOUSEBUTTONDOWN:
                        if (pause == 1)
                        {
                            // Recupère les coordonnees de la cellule cliquee
                            int x, y;
                            SDL_GetMouseState(&x, &y);
                            int row = y / CELL_SIZE;
                            int col = x / CELL_SIZE;

                            // recupère l'emplacement et la taille de la cellule
                            SDL_Rect cellRect = {col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE};

                            // modifie l'etat et la couleur de la cellule
                            if (grid[row][col] == 0) {
                                SDL_SetRenderDrawColor(renderer, COLOR_CELL_HOVER.r, COLOR_CELL_HOVER.g, COLOR_CELL_HOVER.b, COLOR_CELL_HOVER.a);
                                grid[row][col] = 1;
                            } else {
                                SDL_SetRenderDrawColor(renderer, COLOR_CELL.r, COLOR_CELL.g, COLOR_CELL.b, COLOR_CELL.a);
                                grid[row][col] = 0;
                            }

                            SDL_RenderFillRect(renderer, &cellRect);

                            // La grille reste apres le changement de couleur
                            SDL_SetRenderDrawColor(renderer, COLOR_GRID.r, COLOR_GRID.g, COLOR_GRID.b, COLOR_GRID.a);
                            showGrid(renderer);

                            // Affichage des modifications
                            SDL_RenderPresent(renderer);

                            generation = 0;
                        }
                        break;

                    default:
                        break;
                }
            }

            // ----------------------------- Play ----------------------------- //
            while (pause == 0)
            {
                // Desactivation des action de la souris et vidage de la file d'attente
                SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_DISABLE);
                SDL_PumpEvents();

                // Affichage des instructions
                generation ++;
                instructions(generation, pause, wait);

                // Verification de chaque cellule de la grille et mise à jour d'une seconde grille
                for (i = 0; i < GRID_ROWS; i++) {
                    for (j = 0; j < GRID_COLS; j++)
                    {
                        grid2[i][j] = verify(grid, i, j);
                    }
                }

                // Mise à jour de la grille principale avec la seconde et mise à jour de sa couleur 
                for (i = 0; i < GRID_ROWS; i++) {
                    for (j = 0; j < GRID_COLS; j++)
                    {
                        grid[i][j] = grid2[i][j];

                        colorCell(grid, renderer, i, j);
                    }
                }

                // La grille reste apres le changement de couleur
                SDL_SetRenderDrawColor(renderer, COLOR_GRID_INGAME.r, COLOR_GRID_INGAME.g, COLOR_GRID_INGAME.b, COLOR_GRID_INGAME.a);
                showGrid(renderer);

                // Affichage
                SDL_RenderPresent(renderer);



                // Attente d'une action de l'utilisateur
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    switch (event.type) {

                        // ----------------------------- Action du clavier ----------------------------- //
                        case SDL_KEYDOWN:

                            // Pause (Espace)
                            if (event.key.keysym.sym == SDLK_SPACE) {
                                // Remise de la grille a sa couleur d'origine
                                SDL_SetRenderDrawColor(renderer, COLOR_GRID.r, COLOR_GRID.g, COLOR_GRID.b, COLOR_GRID.a);
                                showGrid(renderer);

                                SDL_RenderPresent(renderer);

                                SDL_PumpEvents();
                                pause = 1;

                                // Afficher les instructions
                                instructions(generation, pause, wait);
                            }

                            // Acceleration de la vitesse (Fleche droite)
                            if (event.key.keysym.sym == SDLK_RIGHT && wait != 10) {
                                if (wait > 10)
                                {
                                    wait -= 10;
                                }
                            }

                            // Deceleration de la vitesse (Fleche de gauche)
                            if (event.key.keysym.sym == SDLK_LEFT) {
                                if (wait < 1000)
                                {
                                    wait += 10;
                                }
                            }
                            break;
                        default:
                            break;
                    }
                }
                
                // Attente entre chaque generation
                SDL_Delay(wait);
                
            }
                     
        }
    }

    //Detruire la fenêtre et le renderer
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow( window );

    //Quitte SDL
    SDL_Quit();

    // Liberer l'espace alloue pour les matrices
    freeMemory(grid);
    freeMemory(grid2);

    return 0;
}          