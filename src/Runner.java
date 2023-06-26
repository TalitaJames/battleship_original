import java.util.Map;
import java.util.HashMap;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import java.util.Random;


public class Runner {
    public static List<Board> generateBoardSingle(Ship ship) {

        List<Board> boards = new ArrayList<>();
        boolean[] directions= {true, false};

        for (boolean dir : directions) {
            for (int x = 0; x < Board.getSize(); x++) {
                for (int y = 0; y < Board.getSize(); y++) {
                    
                    Board b = new Board();
                    try { 
                        b.placeShip(ship,Board.coord(x, y),dir);
                        boards.add(b);
                    } 
                    catch (InvalidPlacementException e){}
                    catch(InvalidShipTypeException e){
                        System.err.println("Uhoh! bad ship type!");
                    } 
                    catch(InvalidPositionException e){
                        System.err.println("Uhoh! bad coordinate type!");
                    }
                    
                }
            }
        }
        return boards;
    }

    public static List<Board> addSecondaryShip(List<Board> oldBoards, Ship newShip) {
        
        boolean[] directions= {true, false};
        List<Board> newBoards = new ArrayList<>();
        int numPossible=0;
        int numSuccess=0;


        for (Board oldGrid : oldBoards) {
            for (boolean dir : directions) {
                for (int x = 0; x < Board.getSize(); x++) {
                    for (int y = 0; y < Board.getSize(); y++) {
                        // should make a new copy of the old board (presumably deep)
                        //  then try adding the newShip to it
                        // Board newGrid = Board.deepCopy(oldGrid);
                        
                        Board newGrid = null;
                        try{            
                            newGrid = oldGrid.deepCopy();
                        } catch (Exception e) {
                            System.err.println("deepCopy has errored");
                            System.err.println(e.getStackTrace());
                        }

                        try { 
                            newGrid.placeShip(newShip,Board.coord(x, y),dir);
                            newBoards.add(newGrid);
                            numPossible++;
                            numSuccess++;

                        } 
                        catch (InvalidPlacementException e){
                            numPossible++;
                        } 
                        catch(InvalidShipTypeException e){
                            System.err.println("Uhoh! bad ship type!");
                        } 
                        catch(InvalidPositionException e){
                            System.err.println("Uhoh! bad coordinate type!");
                        }
                        
                    }
                }
            }
        }
        System.out.println("made "+numSuccess+"/"+numPossible);
        return newBoards;
    }
    
    public static void play(Board board){
        // Assuming the board comes prepopulated
        System.out.println("\nBattleship: searching mode");
        int guessCount =0;
        System.out.println(board.toString());

        Scanner sc = new Scanner(System.in);
        while(!board.gameOver()){
            System.out.print("Enter a coordinate: ");
            
            try {
                String input = sc.next(); // clean data so (x,y) and x,y with any space variations work
                String[] result = input.replace('(',' ').replace(')',' ').split(",");
                int x = Integer.parseInt(result[0].trim());
                int y = Integer.parseInt(result[1].trim());
                
                board.attack(Board.coord(x, y));
                guessCount++;

            } catch (InvalidPositionException e) {
                System.out.println("Please enter a valid coordinate in the form \"x,y\" w/ range 0-"+(Board.getSize()-1)+" inclusive");
            }          
            System.out.println(board.toString());
        }
        sc.close();
        System.out.println(" ---- Game over ----\n\tYou made "+guessCount+" guesses");

    }

    public static void deepCopyTest() {

        Ship two = new Ship(2,'2');
        Ship three = new Ship(3,'3');

        List<Board> boards = generateBoardSingle(two);
        // List<Board> moreBoards = addSecondaryShip(boards, three); // This won't work (properly) untill deep copy works
        
        Board foo = boards.get(31);
        Board baz = null;
        
        try{            
            baz = foo.deepCopy();
        } catch (Exception e) {
            System.err.println("deepCopy has errored");
            System.err.println(e.getStackTrace());
        }

        System.out.println(foo.displaySetup());
        System.out.println(baz.displaySetup());


        try {
            baz.placeShip(three, Board.coord(2,1), true); //
        } catch (Exception e) { System.err.println("bad");}
        // System.out.println(moreBoards.size());

        System.out.println(foo.displaySetup()); 
        System.out.println(baz.displaySetup());

        // try {
        //     foo.attack(Board.coord(0, 0));
        // } catch (InvalidPositionException e){}

        // System.out.println(foo.hashCode());
        // System.out.println(foo.toString());

        // System.out.println(baz.hashCode());
        // System.out.println(baz.toString());
    }

    public static void main(String[] args) {

        Ship l2Ship = new Ship(2, '2');
        Ship l3Ship = new Ship(3, '3');

        List<Board> l2Boards = generateBoardSingle(l2Ship);
        List<Board> allBoards = addSecondaryShip(l2Boards, l3Ship);

        System.out.println("l2 "+l2Boards.size()+" and allBoards "+allBoards.size());

        Random rd = new Random();
        for (int i = 0; i < 5; i++) {
            int rdPeak = rd.nextInt(allBoards.size()); // random int to peak at a board
            System.out.println(allBoards.get(rdPeak).displaySetup());
        }
        

    }
}
