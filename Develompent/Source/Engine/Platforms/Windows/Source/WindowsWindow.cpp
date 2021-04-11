#include "WindowsWindow.h"

FWindowsWindow::FWindowsWindow()
{}

FWindowsWindow::~FWindowsWindow()
{
    Close();
}

bool FWindowsWindow::PollEvent()
{
    return false;
}

bool FWindowsWindow::Create( const tchar *InTitle, int32 InWidth, int32 InHeight, EStyle InStyle )
{
    return false;
}

void FWindowsWindow::Close()
{

}