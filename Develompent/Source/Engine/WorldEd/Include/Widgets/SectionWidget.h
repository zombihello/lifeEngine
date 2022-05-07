/**
 * @file
 * @addtogroup WorldEd World editor
 */

/*
    Elypson/qt-collapsible-section
    (c) 2016 Michael A. Voelkel - michael.alexander.voelkel@gmail.com

    This file is part of Elypson/qt-collapsible section.

    Elypson/qt-collapsible-section is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Elypson/qt-collapsible-section is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Elypson/qt-collapsible-section. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SECTIONWIDGET_H
#define SECTIONWIDGET_H

#include <QFrame>
#include <QGridLayout>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QToolButton>
#include <QWidget>

/**
 * @ingroup WorldEd
 * Widget of section
 */
class WeSectionWidget : public QWidget
{
    Q_OBJECT
    
private:
    QGridLayout*                mainLayout;             /**< Main layout */
    QToolButton*                toggleButton;           /**< Toggle button */
    QFrame*                     headerLine;             /**< Header line */
    QParallelAnimationGroup*    toggleAnimation;        /**< Toggle animation */
    QScrollArea*                contentArea;            /**< Content area */
    int                         animationDuration;      /**< Animation duration */
    int                         collapsedHeight;        /**< Collapsed height */
    bool                        isExpanded = false;     /**< Is expanded section */
    
public slots:
    /**
     * Toggle section
     * @param InExpanded      Is expanded section
     */
    void toggle( bool InExpanded );

public:
    static const int DEFAULT_DURATION = 300;      /**< Default duration of animations */

    /**
     * Constructor
     * 
     * @param InTitle               Title of section
     * @param InAnimationDuration   Animation duration
     * @param InParent              Parent widget
     */
    explicit WeSectionWidget( const QString& InTitle = "", const int InAnimationDuration = DEFAULT_DURATION, QWidget* InParent = nullptr );

    /**
     * set layout of content
     * @param InContentLayout       Content layout
     */
    void setContentLayout( QLayout& InContentLayout );
    
    /**
     * Set title
     * @param InTitle       Title
     */
    void setTitle( QString InTitle );

    /**
     * Update animations and their heights
     */
    void updateHeights();

    /**
     * Expand section
     * @param InExpanded      Is expanded section
     */
    void expand( bool InExpanded );
};

#endif // SECTIONWIDGET_H
