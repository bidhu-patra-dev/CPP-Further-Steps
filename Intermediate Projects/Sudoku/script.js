document.addEventListener("DOMContentLoaded", function() {
    generateBoard();
    document.getElementById("solve-button").addEventListener("click", solveSudoku);
    document.getElementById("clear-button").addEventListener("click", clearBoard);
});

// Generate the 9x9 Sudoku grid
function generateBoard() {
    let board = document.querySelector("#sudoku-board tbody");
    for (let i = 0; i < 9; i++) {
        let row = document.createElement("tr");
        for (let j = 0; j < 9; j++) {
            let cell = document.createElement("td");
            cell.contentEditable = "true";
            cell.addEventListener("input", restrictInput);
            row.appendChild(cell);
        }
        board.appendChild(row);
    }
}

function restrictInput(event) {
    let cell = event.target;
    let value = cell.innerText.trim();
    
    if (!/^[1-9]?$/.test(value)) {
        cell.innerText = ""; // Clear invalid input
    }
}

// Convert board to a string format for backend
function boardToString() {
    let tds = document.querySelectorAll("#sudoku-board td");
    return Array.from(tds).map(td => td.innerText.trim() || "-").join("");
}

// Update board with the solved result
function stringToBoard(solution) {
    let tds = document.querySelectorAll("#sudoku-board td");
    solution.split("").forEach((char, i) => {
        tds[i].innerText = char !== "-" ? char : "";
    });
}

function getBackendDomain() {
    const domain = window.location.hostname || window.currentURL.hostname;
    const firstDotIndex = domain.indexOf('.');
    const subdomain = domain.substring(0, firstDotIndex);
    const restOfDomain = domain.substring(firstDotIndex);
    return "https://" + subdomain + "-backend" + restOfDomain;
}

// Fetch solution from backend (C++ server)
async function solveSudoku() {
    try {
        let boardString = boardToString();

        const api = getBackendDomain();
        const response = await axios.post(api + '/solve', { board: boardString });

        if (response.data.solution) {
            stringToBoard(response.data.solution);
        } else {
            alert("Invalid board!");
        }

    } catch (error) {
        console.error("Error fetching solution:", error);
        alert("An error occurred while solving the puzzle. Please check if the board is valid.");
    }
}

// Clear board
function clearBoard() {
    document.querySelectorAll("#sudoku-board td").forEach(td => td.innerText = "");
}