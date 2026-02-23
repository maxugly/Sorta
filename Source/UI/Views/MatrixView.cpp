
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
    const int sqSize = Config::Layout::Matrix::squareSize;
    const int numRows = Config::Layout::Matrix::rows;

    for (size_t i = 0; i < state.ledColors.size(); ++i) {
        int col = static_cast<int>(i) / numRows;
        int row = static_cast<int>(i) % numRows;

        g.setColour(state.ledColors[i]);
        g.fillRect(col * sqSize, row * sqSize, sqSize, sqSize);
    }
}
