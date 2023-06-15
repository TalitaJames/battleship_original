// import java.util.Arrays;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;


public class Runner {

    public static int BOARD_LENGTH=5;
    public static int[] shipSizes = {3,2};

    
    public static List<Ship> createShips(int shipLength, char symbol){
        List<Ship> allShips = new ArrayList<>();
 
        boolean[] directions={true, false}; //  seems silly, but it works

        for (boolean direction : directions) {
            for (int x = 0; x < BOARD_LENGTH; x++) {
                for (int y = 0; y < BOARD_LENGTH; y++) {
                    try {
                        Ship newShip = Ship.createShip(x, y, shipLength, direction, symbol, BOARD_LENGTH);
                        // System.err.println("PASS  for ("+x+", "+y+")\t");
                        allShips.add(newShip);
                    } catch (InvalidPositionException e) {
                        // System.err.println("Error for ("+x+", "+y+")\t"+e.getMessage());
                    }
                }
                
            }
        }
        // System.out.println("length "+ allShips.size()+" of "+posible +" for SL"+shipLength);
        
        return allShips;
    }

    public static void main(String[] args){
        List<Board> boards = new ArrayList<>();
        List<Ship> l3Ships = createShips(3,'3');    
        List<Ship> l2Ships = createShips(2,'2');    

        for (Ship ship3 : l3Ships) {
            for (Ship ship2 : l2Ships) {
                Board newBoard = new Board(BOARD_LENGTH);
                newBoard.addShip(ship3);
                boolean sucsess = newBoard.addShip(ship2);
                if(sucsess){
                    boards.add(newBoard);
                }
            }
        }
        System.out.println("board num "+boards.size());

        // Look at a few boards
        Random rd = new Random(); //(1686782706);
        
        for (int i = 0; i < 2; i++) {
            int rdPeak = rd.nextInt(boards.size()); // storing random integers in an array
            System.out.println(boards.get(rdPeak).toString());
        }

    }
}