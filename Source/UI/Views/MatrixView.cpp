
#include "UI/Views/MatrixView.h"
#include "Utils/Config.h"

MatrixView::MatrixView() {
    setInterceptsMouseClicks(false, false);
}

MatrixView::~MatrixView() = default;

void MatrixView::updateState(const MatrixViewState& newState) {
    state = newState;
    repaint();
}

void MatrixView::paint(juce::Graphics& g) {
    // 1. Draw the Button Background (GroupPosition::Left style)
    auto bounds = getLocalBounds().toFloat().reduced(Config::UI::ButtonOutlineThickness / 2.0f);
    auto cornerSize = Config::UI::ButtonCornerSize;
    
    juce::Path p;
    // Corners: topLeft=true, topRight=false, bottomLeft=true, bottomRight=false
    p.addRoundedRectangle(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(),
                          cornerSize, cornerSize, true, false, true, false);
                          
    g.setColour(Config::Colors::Button::base);
    g.fillPath(p);
    
    g.setColour(Config::Colors::Button::outline);
    g.strokePath(p, juce::PathStrokeType(Config::UI::ButtonOutlineThickness));
    
    // 2. Calculate Centering Offsets
    const int sqSize = Config::Layout::Matrix::squareSize;
    const int numRows = Config::Layout::Matrix::rows;
    
    const int totalLedWidth = 16 * sqSize;
    const int totalLedHeight = numRows * sqSize;
    const int startX = Config::Layout::Matrix::startX; 
    const int startY = (getHeight() - totalLedHeight) / 2;
    
    // 3. Draw the LEDs
    for (size_t i = 0; i < state.ledColors.size(); ++i) {
        int col = static_cast<int>(i) / numRows;
        int row = static_cast<int>(i) % numRows;
        
        g.setColour(state.ledColors[i]);
        g.fillRect(startX + col * sqSize, startY + row * sqSize, sqSize, sqSize);
    }
}
