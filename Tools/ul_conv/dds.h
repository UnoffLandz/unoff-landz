#ifndef DDS_H_INCLUDED
#define DDS_H_INCLUDED

void cleanup (void);
void init ();
void reshape (int w, int h);
void keyboard (unsigned char key, int x, int y);
void display (void);
void display_texture();

#endif // DDS_H_INCLUDED
