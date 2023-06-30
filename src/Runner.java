import java.util.Map;
import java.util.HashMap;
import java.util.List;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Scanner;
import java.io.FileNotFoundException;
import java.io.PrintWriter;
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
        //TODO Could improve later by not checking possitions that are known to go out of map

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
        // System.out.println("added "+numSuccess+"/"+numPossible);
        return newBoards;
    }
    
    public static List<Board> allBoards(List<Ship> fleet) {
        List<Board> allBoards = generateBoardSingle(fleet.get(0));
        // System.out.println("allBoards "+allBoards.size());
        
        if (fleet.size()==1) return allBoards; // if only one ship, early return

        for (int i = 1; i < fleet.size(); i++) {
            allBoards = addSecondaryShip(allBoards, fleet.get(i));
            // System.out.println("allBoards "+allBoards.size());
        }        
        return allBoards;
    }

    // User inputs their guesses for a single board and can "play"
    public static void play(Board board){
        // Assuming the board comes prepopulated
        System.out.println("\nBattleship: searching mode");
        System.out.println(board.toString());

        int guessCount =0;

        Scanner sc = new Scanner(System.in);
        while(!board.gameOver()){
            System.out.print("Enter a coordinate: ");
            boolean success = false;

            guess: try {
                String input = sc.next(); // clean data so (x,y) and x,y with any space variations work
                String[] result = input.replace('(',' ').replace(')',' ').split(",");
                if (result.length != 2){
                    throw new InvalidPositionException();
                }

                int x = Integer.parseInt(result[0].trim());
                int y = Integer.parseInt(result[1].trim());

                if (board.hasBeenHit(Board.coord(x, y))){
                    System.out.println("\tThat possition has been hit already!");
                    break guess;
                }
                
                success = board.attack(Board.coord(x, y));
                guessCount++;

            } catch (InvalidPositionException e) {
                System.out.println("Please enter a valid coordinate in the form \"x,y\" (range is (0,"+(Board.getSize()-1)+") inclusive)");
            } catch (NumberFormatException e){
                System.out.println("Please enter a valid coordinate in the form \"x,y\" (range is (0,"+(Board.getSize()-1)+") inclusive)");
            }

            // update user
            String result = success ? "HIT" : "MISS";
            System.out.println("You "+result);
            System.out.println(board.toString());
        }

        // game over
        sc.close();
        System.out.println(" ---- Game over ----\n\tYou made "+guessCount+" guesses");
    }


    // same as play(Board) except that also narrows down possible board options from guess info
    public static void play(Board board, List<Board> possibleBoards){
        // Assuming the board comes prepopulated
        System.out.println("\nBattleship: searching mode  (there are "+possibleBoards.size()+" possible boards)");
        System.out.println(board.toString());

        int guessCount =0;
        

        Scanner sc = new Scanner(System.in);
        while(!board.gameOver()){
            System.out.print("Enter a coordinate: ");
            boolean success = false;
            guess: try {
                String input = sc.next(); // clean data so (x,y) and x,y with any space variations work
                String[] result = input.replace('(',' ').replace(')',' ').split(",");
                if(result.length != 2){
                    throw new InvalidPositionException();
                }
                int x = Integer.parseInt(result[0].trim());
                int y = Integer.parseInt(result[1].trim());
          
                if (board.hasBeenHit(Board.coord(x, y))){
                    System.out.println("\tThat possition has been hit already!");
                    break guess;
                }
                
                success = board.attack(Board.coord(x, y));
                guessCount++;

                // now purge impossible boards left fromt the result
                List<Board> newPosBoards = new ArrayList<>();

                for (int i = 0; i < possibleBoards.size(); i++) {
                    Board checkBoard = possibleBoards.get(i);
                    if(checkBoard.isOccupied(Board.coord(x, y)) == success){
                        newPosBoards.add(checkBoard);
                    }
                }
                possibleBoards=newPosBoards;
   

            } catch (InvalidPositionException e) {
                System.out.println("Please enter a valid coordinate in the form \"x,y\" (range is (0,"+(Board.getSize()-1)+") inclusive)");
            } catch (NumberFormatException e){
                System.out.println("Please enter a valid coordinate in the form \"x,y\" (range is (0,"+(Board.getSize()-1)+") inclusive)");
            }
            
            

            // update user
            String result = success ? "HIT" : "MISS";
            System.out.println("You "+result+" ("+possibleBoards.size()+" possible boards left)");
            System.out.println(board.toString());

            List<Byte[]> possibleByteBoards = new ArrayList<>();
            for (Board b : possibleBoards) {
                possibleByteBoards.add(b.encodeBoard());
            }

            outputAll(possibleByteBoards, "game0/"+guessCount+"enc.txt");
        }

        // Game over
        sc.close();
        System.out.println(" ---- Game over ----\n\tYou made "+guessCount+" guesses");
    }

    // print them all (mainly for spreadsheet chaos)
    public static void outputAll(List<Byte[]> printLines, String filename) {
        
        try {
            PrintWriter pr = new PrintWriter("../out/"+filename);
            for (Byte[] board : printLines) {
                pr.println(Arrays.toString(board));
            }
            pr.close();
        }
        
        catch (FileNotFoundException e) {
            System.err.println("Error: File not saved");
        }
    }
    
    public static void main(String[] args) {
        List<Ship> fleet = new ArrayList<>(); 
        fleet.add(new Ship(2, '2')); //from .json
        fleet.add(new Ship(3, '3'));

        long startTime = System.currentTimeMillis();
        List<Board> allBoards = allBoards(fleet);
        long endTime = System.currentTimeMillis();

        System.out.println("Setup time: " + (endTime - startTime)+"ms  ("+ (endTime - startTime)/1000+"s)");
        
        Random rd = new Random();
        Board randBoard = allBoards.get(rd.nextInt(allBoards.size()));
        play(randBoard,allBoards);
        


    }
}
