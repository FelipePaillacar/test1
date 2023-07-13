#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
//animacion
//enemigo 
//mejorar mapa 
#define MAP_WIDTH 640
#define a 20
#define MAP_HEIGHT 640
#define PLAYER_SIZE 64
#define PLAYER_SPEED 5
#define MAX_PAPAS 10
struct Rect {
    int x;
    int y;
    int width;
    int height;
};
typedef struct Proyectil {
    float x;
    float y;
    float dx;
    float dy;
    ALLEGRO_BITMAP* imagen;
} Proyectil;
void loadMap(const char* filename, int** map) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("No se pudo abrir el archivo %s\n", filename);
        return;
    }
    for (int i = 0; i < MAP_HEIGHT / PLAYER_SIZE; i++) {
        for (int j = 0; j < MAP_WIDTH / PLAYER_SIZE; j++) {
         fscanf(file, "%d", &map[i][j]);
        }
    }
    fclose(file);
}
void movePlayer(int* playerX, int* playerY, int directionX, int directionY, int** map) {
    int nextX = *playerX + directionX * PLAYER_SPEED;
    int nextY = *playerY + directionY * PLAYER_SPEED;
    if (nextX >= 0 && nextX <= MAP_WIDTH - PLAYER_SIZE &&
        nextY >= 0 && nextY <= MAP_HEIGHT - PLAYER_SIZE &&
        map[nextY / PLAYER_SIZE][nextX / PLAYER_SIZE] != 1) {
        *playerX = nextX;
        *playerY = nextY;
    }
}
bool detectCollision(int** map, int startX, int startY) {
    for (int i = startY; i < startY + 2; i++) {
        for (int j = startX; j < startX + 2; j++) {
            if (map[i][j] != 1) {
                return false;
            }
        }
    }
    return true;
}
void disparar_proyectil(Proyectil* proyectil, float x_inicial, float y_inicial, float dx, float dy) {
    proyectil->x = x_inicial;
    proyectil->y = y_inicial;
    proyectil->dx = dx;
    proyectil->dy = dy;
}
ALLEGRO_BITMAP* crear_imagen_proyectil() {
    ALLEGRO_BITMAP* imagen = al_create_bitmap(10, 10);
    al_set_target_bitmap(imagen);
    al_draw_filled_circle(5, 5, 5, al_map_rgb(255, 0, 0));
    al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));

    return imagen;
}
int main() {
    ALLEGRO_DISPLAY* display = NULL;
    ALLEGRO_EVENT_QUEUE* event_queue = NULL;
    ALLEGRO_EVENT event;
    ALLEGRO_BITMAP* player_image = NULL;
    ALLEGRO_BITMAP* papa_image = NULL;
    ALLEGRO_BITMAP* obstacle_image = NULL;
    if (!al_init()) {
        fprintf(stderr, "Error al inicializar Allegro.\n");
        return -1;
    }
    if (!al_init_image_addon()) {
        fprintf(stderr, "Error al inicializar el complemento de imágenes.\n");
        return -1;
    }

    if (!al_init_primitives_addon()) {
        fprintf(stderr, "Error al inicializar el complemento de primitivas.\n");
        return -1;
    }

    display = al_create_display(MAP_WIDTH, MAP_HEIGHT);
    if (!display) {
        fprintf(stderr, "Error al crear la ventana.\n");
        return -1;
    }

    event_queue = al_create_event_queue();
    if (!event_queue) {
        fprintf(stderr, "Error al crear la cola de eventos.\n");
        al_destroy_display(display);
        return -1;
    }

    if (!al_install_keyboard()) {
        fprintf(stderr, "Error al inicializar el teclado.\n");
        return -1;
    }

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    int** map = (int**)malloc(MAP_HEIGHT / PLAYER_SIZE * sizeof(int*));
    for (int i = 0; i < MAP_HEIGHT / PLAYER_SIZE; i++) {
        map[i] = (int*)malloc(MAP_WIDTH / PLAYER_SIZE * sizeof(int));
    }

    loadMap("map.txt", map);

    player_image = al_load_bitmap("player.png");
    if (!player_image) {
        fprintf(stderr, "Error al cargar la imagen del jugador.\n");
        for (int i = 0; i < MAP_HEIGHT / PLAYER_SIZE; i++) {
            free(map[i]);
        }
        free(map);
        al_destroy_event_queue(event_queue);
        al_destroy_display(display);
        return -1;
    }

    papa_image = al_load_bitmap("papa.png");
    if (!papa_image) {
        fprintf(stderr, "Error al cargar la imagen de la papa.\n");
        for (int i = 0; i < MAP_HEIGHT / PLAYER_SIZE; i++) {
            free(map[i]);
        }
        free(map);
        al_destroy_event_queue(event_queue);
        al_destroy_display(display);
        return -1;
    }

    obstacle_image = al_load_bitmap("obstacle.png");
    if (!obstacle_image) {
        fprintf(stderr, "Error al cargar la imagen del obstáculo.\n");
        for (int i = 0; i < MAP_HEIGHT / PLAYER_SIZE; i++) {
            free(map[i]);
        }
        free(map);
        al_destroy_event_queue(event_queue);
        al_destroy_display(display);
        return -1;
    }

    int playerX = MAP_WIDTH / 2;
    int playerY = MAP_HEIGHT / 2;
    int directionX = 0;
    int directionY = 0;
    int numPapas = 0;
    Proyectil proyectil;
    proyectil.imagen = crear_imagen_proyectil();

    while (1) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (ev.keyboard.keycode) {
            case ALLEGRO_KEY_UP:
                directionY = -1;
                break;
            case ALLEGRO_KEY_DOWN:
                directionY = 1;
                break;
            case ALLEGRO_KEY_LEFT:
                directionX = -1;
                break;
            case ALLEGRO_KEY_RIGHT:
                directionX = 1;
                break;
            case ALLEGRO_KEY_SPACE:
                if (numPapas < MAX_PAPAS) {
                 
                    int playerTileX = playerX / PLAYER_SIZE;
                    int playerTileY = playerY / PLAYER_SIZE;
                    if (map[playerTileY][playerTileX] != 1) {
                        map[playerTileY][playerTileX] = 2;  
                        numPapas++;
                    }
                }
                break;
            case ALLEGRO_KEY_C:
                printf("detecta tecla c :\n");
                disparar_proyectil(&proyectil, 0, 0, 1, 1);
                break;
            }
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            switch (ev.keyboard.keycode) {
            case ALLEGRO_KEY_UP:
            case ALLEGRO_KEY_DOWN:
                directionY = 0;
                break;
            case ALLEGRO_KEY_LEFT:
            case ALLEGRO_KEY_RIGHT:
                directionX = 0;
                break;
            }
        }
        movePlayer(&playerX, &playerY, directionX, directionY, map);
        al_draw_bitmap(al_load_bitmap("map.png"), 0, 0, 0);
        for (int i = 0; i < MAP_HEIGHT / PLAYER_SIZE; i++) {
            for (int j = 0; j < MAP_WIDTH / PLAYER_SIZE; j++) {
                if (map[i][j] == 1) {
                 //  printf("Colisión en la celda (%d, %d)\n", j, i);
                }
                else if (map[i][j] == 2) {
                    al_draw_bitmap(papa_image, j * PLAYER_SIZE, i * PLAYER_SIZE, 0);
                }
               else if (map[i][j] == 0 && i < MAP_HEIGHT / PLAYER_SIZE - 1 && j < MAP_WIDTH / PLAYER_SIZE - 1) {
                    if (detectCollision(map, j, i)) {
                //       printf("Espacio 2x2 encontrado en (%d, %d)\n", j, i);
                        al_draw_bitmap(obstacle_image, j * PLAYER_SIZE, i * PLAYER_SIZE, 0);
                 }
              }
           }
      }
        al_draw_bitmap(player_image, playerX, playerY, 0);
        al_flip_display();
    }
    al_destroy_bitmap(player_image);
    al_destroy_bitmap(papa_image);
    al_destroy_bitmap(obstacle_image);
    for (int i = 0; i < MAP_HEIGHT / PLAYER_SIZE; i++) {
        free(map[i]);
    }
    free(map);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    return 0;
}