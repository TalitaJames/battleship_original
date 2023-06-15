import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.HashMap;
import java.util.HashSet;

public class Board {
    private static int SIZE; //TODO final break multiple boards?
    private List<Ship> ships;

    private final Map<String, Boolean> board;
    private final Map<String, Character> boardChar;


    public Board(int boardLength) {
        Board.SIZE = boardLength; // revisit when thinking abt gridding

        board = new HashMap<>();
        boardChar = new HashMap<>();

        for (int x = 0; x <= Board.SIZE; x++) {
            for (int y = 0; y <= Board.SIZE; y++) {
                board.put(coord(x,y), false);                
                boardChar.put(coord(x,y), '.');
            }
        }
        // System.out.println(board);

        this.ships = new ArrayList<>();

    }


    // addShip method - Checks for intersection, only adds if false
    // FIXME: not entierly convinced this is correct, 962 for the set method, the bool[][] was 956
    public boolean addShip(Ship newShip) {
        char[] bannedSymbols = {'[',']','.','X','x','O'};

        for (char badSym : bannedSymbols) {
            if(Character.compare(badSym, newShip.getSymbol())==0){
                return false; // should probs throw an error "BAD SYMBOL or something"
            }
        }


        for (Ship ship : ships) {
            Set<String> coordIntersects = new HashSet<>(ship.getCoords()); // copy of ship coords to new set
            coordIntersects.retainAll(newShip.getCoords());
            if(coordIntersects.size()>0){
                return false;
            }
        }

        // no intersection -> add the ship
        ships.add(newShip);
    
        for(String newSpot: newShip.getCoords()){ //and update grid
            board.put(newSpot, true);
            boardChar.put(newSpot, newShip.getSymbol());
        }

        return true;
    }


    @Override
    public String toString() {
        String strGrid = "";
        
        for (int y = 0; y <= Board.SIZE; y++) {
            strGrid += "[";
            for (int x = 0; x <= Board.SIZE; x++) {
                // String (x, y)
                // String value = board.get(coord(x,y)) ? "X" : ".";
                String value = String.valueOf(boardChar.get(coord(x,y)));
                strGrid += value + " ";
                

            }
            strGrid += "]\n";
        }
        return strGrid;
    }

    public static String coord(int x, int y){
        return "("+x+", "+y+")";
    }
}
