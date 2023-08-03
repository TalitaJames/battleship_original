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
    public static void main(String[] args) {
        Ship[] fleet = parseSettings(args);

        byte byteMin = 0; // because the 0 to 255 thing, not -128 to 127
        int uByteMax = ((Board.getLength()-1) * 11 << 1); //unsigned byte max
        byte byteMax = (byte) uByteMax;

        int threadCount = 6;
        Thread[] ta = new Thread[threadCount]; //ta (thread array),ba (byte array), tg (thread group), lg (lifes good)
        ByteIterator[] ba = new ByteIterator[threadCount];
        ThreadGroup tg = new ThreadGroup("Mission A");  


        int secSize = (int) Math.floor(uByteMax/threadCount); // each section size

        long startTime = System.currentTimeMillis();
    
        // create each thread for that subset of values
        for(int i=0; i<ta.length;i++){
            int secMin=secSize*i;
            int secMax=secSize*(i+1);
            int endByteMax =  secMax>uByteMax ? byteMax : secMax; // if it would go over board max lim, don't

            // System.out.println("\tMin:"+secMin+" Max:"+secMax);
            ba[i] = new ByteIterator((byte) secMin,(byte) endByteMax, fleet);
            ta[i] = new Thread(tg,ba[i]);
            ta[i].start();
        }
        
        while(tg.activeCount()>0){
            try{Thread.sleep(1);} catch(InterruptedException e){}
        }
        long endTime = System.currentTimeMillis();

        int goodBoards = 0;
        for(ByteIterator b : ba){
            goodBoards+=b.getShipCount();
        }

        String timeCSV = (endTime - startTime)+",";
        String countCSV = goodBoards+",";


        // append status to a file
        File fTime = new File("../timeTesting/results_timeData_java.txt");
        File fCount = new File("../timeTesting/results_shipCount.txt");
		try (FileWriter frTime = new FileWriter(fTime, true);
             FileWriter frCount = new FileWriter(fCount, true)){			
			frTime.write(timeCSV);
            frCount.write(countCSV);
		} catch (IOException e) {
			e.printStackTrace();
		}
           
    }


    // ---- Generating Board Byte Methods
    public static Byte[] nextByte(Byte[] data){
        // because the last number of bytes are superfluous, they don't need to be iterated
        int maxCoord = (Board.getLength()-1)*11;
        Byte byteMax = (byte) (maxCoord << 1);
        Byte byteMin = 0; // because the 0 to 255 thing, not -128 to 127
        
        boolean validByte = false;
        
        while(!validByte){
            // Check if at the end of the values
            boolean endVal = true;
            int n = 0;
            while(endVal && n<data.length){
                endVal = (data[n]==byteMax);
                n++;
            }
            if(endVal) return null;

       
            // iterate the value
            for (int i = 0; i < data.length; i++) {
                if (data[i]==byteMax){
                    data[i]=byteMin;
                } else{
                    data[i]++;
                    break;
                }
            }

            // checks if is a valid byte[] (ie all bytes are unique and none are illegal)
            if(!(Arrays.stream(data).distinct().count() < data.length)) validByte = true;
        }
    
        return data;
    }

    public static void iterateBytes(Ship[] fleet){
        
        
        Byte[] encodedBoard = new Byte[fleet.length];
        for (int i = 0; i < encodedBoard.length; i++) encodedBoard[i]=0; // Starts at zero, so next() method iterates thru binary

        int allBoards=0;
        int goodBoards=0;

        long startTime = System.currentTimeMillis();
        while(encodedBoard!=null){
            try {
                Board test = Board.decodeBoard(encodedBoard, fleet);
                goodBoards++;
                // System.out.print(Arrays.toString(encodedBoard));
            } catch (InvalidIntersectionException e){
                // System.err.print(" Intersection");
            } catch (InvalidPlacementException e) {
                // System.err.print(" Placement");
            } catch (InvalidShipTypeException e) {
                System.err.println("Uhoh! Ship Type is wrong");
                System.err.println(e.getStackTrace());
            } catch (InvalidPositionException e){
                // System.err.println("Uhoh: Bad coordinate");
            }

            allBoards++;
            // if(allBoards%1e7==0) System.out.println("done "+Arrays.toString(encodedBoard)+" max byte ("+(byte) ((Board.getLength()-1) * 11 << 1)+")");
            encodedBoard = nextByte(encodedBoard);
            // System.out.println("Checking "+Arrays.toString(encodedBoard));
        }
        long endTime = System.currentTimeMillis();
        

        
        String timeCSV = (endTime - startTime)+",";
        String countCSV = goodBoards+",";


        // append status to a file
        File fTime = new File("../timeTesting/results_timeData_java.txt");
        File fCount = new File("../timeTesting/results_shipCount.txt");
		try (FileWriter frTime = new FileWriter(fTime, true);
             FileWriter frCount = new FileWriter(fCount, true)){			
			frTime.write(timeCSV);
            frCount.write(countCSV);
		} catch (IOException e) {
			e.printStackTrace();
		}
        
    }

    
    // ---- Generating Board Obj Methods

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
    private static Ship[] parseSettings(String[] args) {
        Ship[] fleet;
        int boardSize = 5; // default
       
        if (args.length == 0) {
            fleet = new Ship[2]; 
            fleet[0] = new Ship(3, '3');
            fleet[1] = new Ship(2, '2');
            Board.setBoardSize(boardSize);
            return fleet;
        }

        else if(args.length == 2){ // the input is like this "[3:a, 2:z, 4:w]" (where its length:char)
        
            try {
                boardSize = Integer.parseInt(args[0]);
            } catch (NumberFormatException e) {
                System.err.println("! Invalid input args ! (size must be an int)");
                System.exit(0);
            }

            try {
                String[] ships = args[1].replace("[", "").replace("]", "").split(",");
                fleet = new Ship[ships.length];
                for(int i=0; i<ships.length; i++){

                    ships[i] = ships[i].replaceAll("\\s", ""); // remove all whitespace
                    String[] ship = ships[i].split(":"); // split into length and char
                    fleet[i]=new Ship(Integer.parseInt(ship[0]), ship[1].charAt(0));
                }
                
                Board.setBoardSize(boardSize);
                return fleet;
            } catch (Exception e) {
                System.err.println("! Invalid input args !");
                System.exit(0);
            }
        }
        else{
            System.err.println("! Invalid input args ! (must be 2 args: size and [fleet])");
            System.exit(0);
        }

        return null; // there isn't any way it could get here, but to apease the compiler 
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
    
    
}
