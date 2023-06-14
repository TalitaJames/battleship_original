// import java.util.Arrays;
import java.util.ArrayList;
import java.util.List;


public class Runner {

    public static int BOARD_LENGTH=5;
    public static int[] shipSizes = {3,2};



//     // i think this is equivilant to the `positions()` python code in `yrs_battleship.py` (with flatten=true)
//     public static List<boolean[][]> positions(int shipLength) {
        
//         int singleRowPlacements = (BOARD_LENGTH-shipLength+1);
//         int placements = 2*BOARD_LENGTH*singleRowPlacements; // the # of possible possitions for the ship to be in the grid
        
//         // all the different single row arangements 
//         List<boolean[]> singleRowlocations = new ArrayList<boolean[]>();
//         for (int i = 0; i < singleRowPlacements; i++) {
//             boolean[] singleRow = new boolean [BOARD_LENGTH];
//             Arrays.fill(singleRow, i, i+shipLength, true);
//             singleRowlocations.add(singleRow);
//         }
        
//         List<boolean[][]> locations = new ArrayList<boolean[][]>(); // list to be filled w/all legal ship positions 

//         // horizontal positions
//         for (int i = 0; i < BOARD_LENGTH; i++) {
//             for (int j = 0; j < singleRowlocations.size(); j++) {
//                 boolean[][] location = new boolean [BOARD_LENGTH][BOARD_LENGTH];
//                 location[i]=singleRowlocations.get(j);

//                 locations.add(location);
//             }   
//         }

//         // vertical positions (the transposed of all the possible horizontal pos)
//         for (int i = 0; i < placements/2; i++) {
//             // placements doesn't have to be used here, and i think it makes more sense to save the current size of locations and base it of that
//             locations.add(transpose(locations.get(i)));
//         }

//         return locations;
//     } 
    
    public static List<Ship> createShips(int shipLength, char symbol){
        List<Ship> allShips = new ArrayList<>();
        int posible = 0;
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
                    posible++;
                }
                
            }
        }


        System.out.println("length "+ allShips.size()+" of "+posible +" for SL"+shipLength);
        
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
        System.out.println(boards.size());
        
        System.out.println(boards.get(721).toString());


        // Board grid = new Board(BOARD_LENGTH);
        // try {
        //     System.out.println(grid.toString());
        //     Ship newShip = Ship.createShip(1, 1, 3, true, 'T', BOARD_LENGTH);
        //     grid.addShip(newShip);
        //     System.out.println(grid.toString());
        //     Ship anotherShip = Ship.createShip(3, 0, 2, false, 'F', BOARD_LENGTH);
        //     grid.addShip(anotherShip);
        //     System.out.println(grid.toString());
           

        // } catch (Exception e) {
        //     // TODO: handle exception
        // }

    }
}