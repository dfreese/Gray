#include <PixelArray/OpenGlRgbImage.h>

#if defined(_WIN32)			// If on windows, need this for gl.h
#include <windows.h>
#endif  // defined(_WIN32)
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

OpenGlRgbImage::OpenGlRgbImage(int numRows, int numCols) :
        RgbImage(numRows, numCols)
{

}

// Load the bitmap from the current OpenGL buffer
bool OpenGlRgbImage::LoadFromOpenglBuffer()
{
    int viewportData[4];
    glGetIntegerv( GL_VIEWPORT, viewportData );
    int& vWidth = viewportData[2];
    int& vHeight = viewportData[3];

    if ( ImagePtr==0 ) { // If no memory allocated
        NumRows = vHeight;
        NumCols = vWidth;
        ImagePtr = new unsigned char[NumRows*GetNumBytesPerRow()];
        if ( !ImagePtr ) {
            fprintf(stderr, "Unable to allocate memory for %ld x %ld buffer.\n",
                    NumRows, NumCols);
            Reset();
            ErrorCode = MemoryError;
            return false;
        }
    }
    assert ( vWidth>=NumCols && vHeight>=NumRows );
    int oldGlRowLen;
    if ( vWidth > NumCols ) {
        glGetIntegerv( GL_PACK_ROW_LENGTH, &oldGlRowLen );
        glPixelStorei( GL_PACK_ROW_LENGTH, NumCols );
    }
    glPixelStorei(GL_PACK_ALIGNMENT, 4);

    // Get the frame buffer data.
    glReadPixels( 0, 0, NumCols, NumRows, GL_RGB, GL_UNSIGNED_BYTE, ImagePtr);

    // Restore the row length in glPixelStorei  (really ought to restore alignment too).
    if ( vWidth > NumCols ) {
        glPixelStorei( GL_PACK_ROW_LENGTH, oldGlRowLen );
    }
    ErrorCode = NoError;
    return true;
}

// Draw the bitmap into the current OpenGL buffer
//    Always starts at the position (0,0).
bool OpenGlRgbImage::DrawToOpenglBuffer()
{
    int viewportData[4];
    glGetIntegerv( GL_VIEWPORT, viewportData );
    int& vWidth = viewportData[2];
    int& vHeight = viewportData[3];

    if ( !ImageLoaded() ) { // If no memory allocated
        assert ( false && "RgbImage.cpp error: No RGB Image for DrawToOpenglBuffer()." );
        ErrorCode = WriteError;
        return false;
    }

    assert ( vWidth>=NumCols && vHeight>=NumRows );
    int oldGlRowLen;
    if ( vWidth > NumCols ) {
        glGetIntegerv( GL_UNPACK_ROW_LENGTH, &oldGlRowLen );
        glPixelStorei( GL_UNPACK_ROW_LENGTH, NumCols );
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    // Upload the frame buffer data.
    glRasterPos2i(0,0);		// Position at base of window
    glDrawPixels( NumCols, NumRows, GL_RGB, GL_UNSIGNED_BYTE, ImagePtr);

    // Restore the row length in glPixelStorei  (really ought to restore alignment too).
    if ( vWidth > NumCols ) {
        glPixelStorei( GL_UNPACK_ROW_LENGTH, oldGlRowLen );
    }
    ErrorCode = NoError;
    return true;
}
