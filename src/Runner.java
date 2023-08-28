import java.util.List;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Scanner;
import java.util.Random;
import java.util.Collections;
import java.util.Set;
import java.util.HashSet;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.io.FileNotFoundException;
import java.io.IOException;


public class Runner {
    private static Ship[] fleet;
    private static int threadCount;

    public static void main(String[] args) {
        parseSettings(args, false);
        JobQueue.initJobQueue();
        createThreads();
    }

    // ---- Generating Board Obj Methods
    public static long createThreads(){

        // sets up the threads
        // ta (thread array), ba (ByteIterator array), tg (thread group), lg (lifes good)
        Thread[] ta = new Thread[threadCount]; 
        ByteIterator[] ba = new ByteIterator[threadCount];
        ThreadGroup tg = new ThreadGroup("Mission A");  

        long startTime = System.currentTimeMillis();    
        // create each thread for that subset of values
        for(int i=0; i<threadCount; i++){
            ba[i] = new ByteIterator(fleet);
            ta[i] = new Thread(tg,ba[i]);
            ta[i].start();
        }
        
        // prevents trying to do other things while threads run
        while(tg.activeCount()>0){
            try{Thread.sleep(1);} catch(InterruptedException e){}
        }
        long endTime = System.currentTimeMillis();

        // all the threads are done, so sum their results
        long goodBoards = 0;
        for(int i=0; i<ba.length; i++){
            goodBoards+=ba[i].getShipCount();
        }
        System.out.println(goodBoards);
        
        // append status to file
        String timeCSV = (endTime - startTime)+",";
        String countCSV = goodBoards+",";

        File fTime = new File("../timeTesting/results_timeData_java.txt");
        File fCount = new File("../timeTesting/results_shipCount.txt");
		try (FileWriter frTime = new FileWriter(fTime, true);
             FileWriter frCount = new FileWriter(fCount, true)){			
			frTime.write(timeCSV);
            frCount.write(countCSV);
		} catch (IOException e) {
			e.printStackTrace();
		}
        return goodBoards;
    }

    public static List<Board> allBoards(Ship[] fleet) {
        List<Board> allBoards = generateBoardSingle(fleet[0]);
        
        if (fleet.length==1) return allBoards; // if only one ship, early return

        for (int i = 1; i < fleet.length; i++) {
            allBoards = addSecondaryShip(allBoards, fleet[i]);
        }        
        return allBoards;
    }

    private static List<Board> generateBoardSingle(Ship ship) {
        boolean[] directions= {true, false};
        List<Board> boards = new ArrayList<>();

        for (boolean dir : directions) {
            for (int x = 0; x < Board.getLength(); x++) {
                for (int y = 0; y < Board.getLength(); y++) {
                    
                    Board b = new Board();
                    try { 
                        b.placeShip(ship,Board.coord(x, y),dir);
                        boards.add(b);
                    } 
                    catch (InvalidPlacementException e){}
                    catch (InvalidIntersectionException e){}
                    catch(InvalidShipTypeException e){
                        System.err.println("Uhoh! Bad ship type!");
                    } 
                    catch(InvalidPositionException e){
                        System.err.println("Uhoh! Bad coordinate type!");
                    }
                    
                }
            }
        }
        return boards;
    }

    private static List<Board> addSecondaryShip(List<Board> oldBoards, Ship newShip) {        
        boolean[] directions= {true, false};
        List<Board> newBoards = new ArrayList<>();

        for (Board oldGrid : oldBoards) {
            for (boolean dir : directions) {
                for (int x = 0; x < Board.getLength(); x++) { 
                    for (int y = 0; y < Board.getLength(); y++) {
                        
                        Board newGrid = null;
                        try{            
                            newGrid = oldGrid.deepCopy();
                        } catch (Exception e) {
                            System.err.println("Uhoh! deepCopy has errored");
                            System.err.println(e.getStackTrace());
                        }

                        try { 
                            newGrid.placeShip(newShip,Board.coord(x, y),dir);
                            newBoards.add(newGrid);
                        } 
                        catch (InvalidPlacementException e){} 
                        catch (InvalidIntersectionException e){} 
                        catch(InvalidShipTypeException e){
                            System.err.println("Uhoh! Bad ship type!");
                        } 
                        catch(InvalidPositionException e){
                            System.out.println("Uhoh! Bad coordinate type!");
                        }
                        
                    }
                }
            }
        }
        return newBoards;
    }
    

    // ---- Play/Interaction Methods
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

            } catch (InvalidPositionException e) {
                System.err.println("Please enter a valid coordinate in the form \"x,y\" (range is (0,"+(Board.getLength()-1)+") inclusive)");
            } catch (NumberFormatException e){
                System.err.println("Please enter a valid coordinate in the form \"x,y\" (range is (0,"+(Board.getLength()-1)+") inclusive)");
            }

            // update user
            String result = success ? "HIT" : "MISS";
            System.out.println("You "+result);
            System.out.println(board.toString());
        }

        // Game over
        sc.close();
        System.out.println(" ---- Game over ----\n\tYou made "+guessCount+" guesses");
    }

    // same as play(Board) and narrows down possible board options from guess info
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
   

            } catch (InvalidPositionException e) {    // ---- Generating Board Methods
                System.err.println("Please enter a valid coordinate in the form \"x,y\" (range is (0,"+(Board.getLength()-1)+") inclusive)");
            } catch (NumberFormatException e){
                System.err.println("Please enter a valid coordinate in the form \"x,y\" (range is (0,"+(Board.getLength()-1)+") inclusive)");
            }
            
            

            // update user
            String result = success ? "HIT" : "MISS";
            System.out.println("You "+result+" ("+possibleBoards.size()+" possible boards left)");
            System.out.println(board.toString());

            List<Byte[]> possibleByteBoards = new ArrayList<>();
            for (Board b : possibleBoards) {
                possibleByteBoards.add(b.encodeBoard());
            }

            outputAllBytes(possibleByteBoards, guessCount+"enc.txt");
        }

        // Game over
        sc.close();
        System.out.println(" ---- Game over ----\n\tYou made "+guessCount+" guesses");
    }


    // ---- IO Methods
    private static void parseSettings(String[] args, boolean verbose) {
        int boardSize = 5; // default
        threadCount = 1;
       
        // no args, use defult paramaters
        if (args.length == 0) {
            fleet = new Ship[2]; 
            fleet[0] = new Ship(3, '3');
            fleet[1] = new Ship(2, '2');
            Board.setBoardSize(boardSize);
        }

        // input is given in the form: boardSize fleetInfo threadCount
        // fleet is "[3:a, 2:z, 4:w]" (where its length:char) 
        else if(args.length == 3){ 

            // board length
            try {
                boardSize = Integer.parseInt(args[0]);
                if(0>=boardSize || boardSize>12) {
                    closeProgram("! Invalid input args ! (boardSize must be 0<size<13)");
                }
            } catch (NumberFormatException e) {
                closeProgram("! Invalid input args ! (boardSize must be an int)");
            }

            //fleetInfo
            try {
                String[] ships = args[1].replace("[", "").replace("]", "").split(",");
                fleet = new Ship[ships.length];
                for(int i=0; i<ships.length; i++){

                    ships[i] = ships[i].replaceAll("\\s", ""); // remove all whitespace
                    String[] ship = ships[i].split(":"); // split into length and char
                    fleet[i]=new Ship(Integer.parseInt(ship[0]), ship[1].charAt(0));
                }
                
                Board.setBoardSize(boardSize);
            } catch (Exception e) {
                closeProgram("! Invalid input args ! fleet must be \"[3:a, 2:z, 4:w]\" (where its length:char) ");
            }

            //threadCount
            try{
                threadCount = Integer.parseInt(args[2]); //FIXME: implement error checking later (& move to parse?)
                if(0>=threadCount) {
                    closeProgram("! Invalid input args ! (threadCount must be 0<threadCount)");
                }
            } catch(NumberFormatException e){
                closeProgram("! Invalid input args ! (threadCount must be an int)");
            }
        }
        
        // something else has gone wrong
        else{
            closeProgram("! Invalid input args ! (must have args: size [fleet] threadCount)");
        }

        // prints system status
        if(verbose){
            System.out.println("System running with: \n"+
                            "\tBoardSize: "+Board.getLength()+
                            "\tfleetSize: "+fleet.length+
                            "\tthreadCount: "+threadCount);
        }

    }
    
    public static void closeProgram(){
        closeProgram("Fatal Error! Closing program!");
    }

    public static void closeProgram(String errorMsg){
        System.err.println(errorMsg);
        System.exit(0);
    }

    // Saves a file of bytes (each line is a board)
    public static void outputAllBytes(List<Byte[]> printLines, String filename) {
        try {
            PrintWriter pr = new PrintWriter("../out/game1/"+filename);
            for (Byte[] board : printLines) {
                pr.println(Arrays.toString(board));
            }
            pr.close();
        }
        catch (FileNotFoundException e) {
            System.err.println("Error: File not saved?");
        }
    }

    public static void outputAllGrids(List<Board> printLines, String filename) {
        try {
            PrintWriter pr = new PrintWriter("../out/"+filename);
            for (Board board : printLines) {
                pr.println(board.displaySetup());
            }
            pr.close();
        }
        
        catch (FileNotFoundException e) {
            System.err.println("Error: File not saved");
        }
    }

    public static List<Byte[]> inputBytes(String filename) {
        List<Byte[]> inputList = new ArrayList<Byte[]>();
        int numberShips = 2; // from input args

        try {
            BufferedReader br = new BufferedReader(new FileReader(filename));

            while (br.ready()) {
                String input = br.readLine();

                String[] result = input.replace('[',' ').replace(']',' ').split(",");
                if(result.length != numberShips){
                    // throw new Exception();
                }
                Byte[] shipBytes = new Byte[numberShips];

                for (int i = 0; i < shipBytes.length; i++) {
                    try {
                        int shipInt = Integer.parseInt(result[i].trim());
                        if (-128>shipInt || shipInt>127){
                            throw new NumberFormatException();
                        }
                        shipBytes[i]= (byte) shipInt;
                        
                    } catch (NumberFormatException e) {
                        System.err.println("Something went wrong in the byte conversion!");
                    }
                }
                inputList.add(shipBytes);

            }

            br.close();
        }
        catch (FileNotFoundException e) {
            System.err.println("Uhoh! " + filename + " doesn't exist.");
        }
        catch (IOException e) {
            System.err.println("Uhoh! Something went wrong on the read.");
        }
        return inputList;
    }
    
    public static int getFleetLength(){
        return fleet.length;
    }
    
}
