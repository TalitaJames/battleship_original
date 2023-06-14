import java.util.ArrayList;
import java.util.List;

public class Board {
    // Needs a size (static)
    // list of ships
    // a 2d array?
    private static int BOARD_LENGTH;
    private List<Ship> ships;



    public Board(int boardLength) {
        Board.BOARD_LENGTH=boardLength; // revisit when thinking abt gridding
        this.ships = new ArrayList<>();
    }



    // addShip method - Checks for intersection, only adds if false

    
    @Override
    public String toString(){
        boolean[][] printMe = new boolean [BOARD_LENGTH][BOARD_LENGTH]; // i mean everntually this would have ships in it


        String stringGrid="";
        
        for (boolean[] gridRow : printMe) {
            stringGrid+="[ ";
            for (boolean cellValue : gridRow) {
                char symbol ='.';
                if(cellValue){
                    symbol='X';
                }
                stringGrid+=symbol+" ";
                
            }
            stringGrid+="]\n";
        }


        return stringGrid;
    }

}
