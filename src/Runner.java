import java.util.List;
import java.util.ArrayList;
import java.util.Scanner;

import javax.security.auth.x500.X500Principal;

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

        for (Board oldGrid : oldBoards) {
            
            
            for (boolean dir : directions) {
                for (int x = 0; x < Board.getSize(); x++) {
                    for (int y = 0; y < Board.getSize(); y++) {
                        // FIXME: should make a new copy of the old board (presumably deep)
                        //  then try adding the newShip to it
                        Board newGrid = Board.deepCopy(oldGrid); //FIXME this returns blank
                        try { 
                            newGrid.placeShip(newShip,Board.coord(x, y),dir);
                            newBoards.add(newGrid);
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
        }

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

    public static Board chaos() {

        List<Ship> fleet = new ArrayList<>();
        Ship two = new Ship(2,'2');
        Ship three = new Ship(3,'3');
        fleet.add(two);
        fleet.add(three);
        
        List<Board> boards = generateBoardSingle(two);
        // List<Board> moreBoards = addSecondaryShip(boards, three);
        
        Board foo = boards.get(31);
        Board baz = Board.deepCopy(foo);
        // System.out.println(foo.displaySetup());

        try {
            baz.placeShip(three, Board.coord(2,1), true);
        } catch (Exception e) { System.err.println("bad");}
        // System.out.println(moreBoards.size());

        return baz;
        // System.out.println(foo.displaySetup());
        // System.out.println(baz.displaySetup());
    }

    public static void main(String[] args) {
        Board foo = chaos();
        play(foo);
    }
}
