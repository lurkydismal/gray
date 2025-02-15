#define GLFW_INCLUDE_NONE
#define GLAD_GL_IMPLEMENTATION

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#define TARGET_FPS 60
#define ONE_MILLISECOND_IN_NANOSECONDS 1000000
#define MILLISECONDS_TO_NANOSECONDS( _milliseconds ) \
    ( _milliseconds * ONE_MILLISECOND_IN_NANOSECONDS )

static size_t g_fps = 0;
static bool g_exit = false;

void* t( void* data ) {
    static size_t l_fps = 0;

    ( void )( sizeof( data ) );

    while ( !g_exit ) {
        printf( "%lu\n", ( g_fps - l_fps ) );

        l_fps = g_fps;

        sleep( 1 );
    }

    return ( NULL );
}

int main( void ) {
    GLFWwindow* window;

    /* Initialize the library */
    if ( !glfwInit() )
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow( 640, 480, "Hello World", NULL, NULL );
    if ( !window ) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent( window );

    int version = gladLoadGL( glfwGetProcAddress );
    printf( "GL %d.%d\n", GLAD_VERSION_MAJOR( version ),
            GLAD_VERSION_MINOR( version ) );

    struct timespec next_frame;

    next_frame.tv_sec = 0;
    next_frame.tv_nsec = MILLISECONDS_TO_NANOSECONDS( 16.6666667 );

    pthread_t tr;

    if ( pthread_create( &tr, NULL, t, NULL ) ) {
        printf( "error: \n" );
    }

    for ( ;; ) {
        {
            /* Render here */
            glClear( GL_COLOR_BUFFER_BIT );

            /* Swap front and back buffers */
            glfwSwapBuffers( window );

            /* Poll for and process events */
            glfwPollEvents();
        }

        {
            g_fps++;

            clock_nanosleep( CLOCK_MONOTONIC, 0, &next_frame, NULL );
        }

        if ( glfwWindowShouldClose( window ) ) {
            g_exit = true;

            break;
        }
    }

    pthread_join( tr, NULL );

    glfwTerminate();

    return ( 0 );
}
