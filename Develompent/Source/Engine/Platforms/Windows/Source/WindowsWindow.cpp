#include "WindowsWindow.h"

WindowsWindow::WindowsWindow()
{}

WindowsWindow::~WindowsWindow()
{
    Close();
}

bool WindowsWindow::PollEvent()
{
    return false;
}

bool WindowsWindow::Create( const tchar *InTitle, int32 InWidth, int32 InHeight, EStyle InStyle )
{
    return false;
}

void WindowsWindow::Close()
{

}