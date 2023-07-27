import java.util.List;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Scanner;
import java.util.Random;
import java.util.Collections;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.io.FileNotFoundException;
import java.io.IOException;


public class Runner {

    public static void main(String[] args) {
        List<Ship> fleet = parseSettings(args);
        Collections.sort(fleet);
        
        long startTime = System.currentTimeMillis();
        Byte[] encodedBoard = new Byte[fleet.size()];
        for (int i = 0; i < encodedBoard.length; i++) encodedBoard[i]=0; // Starts at zero, so next() method iterates thru binary

        int allBoards=0;
        int goodBoards=0;
        
        while(encodedBoard!=null){
            try {
                System.out.print(Arrays.toString(encodedBoard));
                String s1=String.format("%8s", Integer.toBinaryString((int) encodedBoard[0] & 0xFF)).replace(' ', '0');
                System.out.println("\t0b"+s1);

                Board test = Board.decodeBoard(encodedBoard, fleet);
                goodBoards++;

            } catch (InvalidPlacementException e) {
                // System.out.println("Uhoh: Bad ship placment");
            } catch (InvalidShipTypeException e) {
                System.out.println("Uhoh! Ship Type is wrong");
                System.out.println(e.getStackTrace());
            } catch (InvalidPositionException e){
                // System.out.println("Uhoh: Bad coordinate");
            }

            allBoards++;
            if(allBoards%1e7==0) System.out.println("done "+Math.round(allBoards/1e7)+"/"+Math.round(Math.pow(255,fleet.size())/1e7));

            encodedBoard = nextByte(encodedBoard);
        }
        long endTime = System.currentTimeMillis();
        
        // String timeCSV = (endTime - startTime)+",";
        // String countCSV = goodBoards+",";
        System.out.println(allBoards);


        // // append status to a file
        // File fTime = new File("../timeTesting/results_timeData_java.txt");
        // File fCount = new File("../timeTesting/results_shipCount.txt");
		// try (FileWriter frTime = new FileWriter(fTime, true);
        //      FileWriter frCount = new FileWriter(fCount, true)){			
		// 	frTime.write(timeCSV);
        //     frCount.write(countCSV);
		// } catch (IOException e) {
		// 	e.printStackTrace();
		// }
        
    }


    // ---- Generating Board Methods
    public static Byte[] nextByte(Byte[] data){
        // Byte byteMax = -1; //largest binary val (0b11111111)

        // because the last number of bytes are superfluous, they don't need to be itterated
        // except they do, when the order is -128 to 127 (signed)
        int maxCoord = (Board.getLen()-1)*11;
        Byte byteMax = (byte) (maxCoord << 1);

        boolean endVal = true;
        for (Byte b : data) {
            if(b != byteMax) endVal = false;          
        }
        if(endVal) return null; // at the end of the values

        for (int i = 0; i < data.length; i++) {
            if (data[i]==byteMax){
                data[i]++;
            } else{
                data[i]++;
                return data;
            }
        }
        return data;
    }

    public static List<Board> allBoards(List<Ship> fleet) {
        List<Board> allBoards = generateBoardSingle(fleet.get(0));
        
        if (fleet.size()==1) return allBoards; // if only one ship, early return

        for (int i = 1; i < fleet.size(); i++) {
            allBoards = addSecondaryShip(allBoards, fleet.get(i));
        }        
        return allBoards;
    }

    private static List<Board> generateBoardSingle(Ship ship) {
        boolean[] directions= {true, false};
        List<Board> boards = new ArrayList<>();

        for (boolean dir : directions) {
            for (int x = 0; x < Board.getLen(); x++) {
                for (int y = 0; y < Board.getLen(); y++) {
                    
                    Board b = new Board();
                    try { 
                        b.placeShip(ship,Board.coord(x, y),dir);
                        boards.add(b);
                    } 
                    catch (InvalidPlacementException e){}
                    catch(InvalidShipTypeException e){
                        System.out.println("Uhoh! Bad ship type!");
                    } 
                    catch(InvalidPositionException e){
                        System.out.println("Uhoh! Bad coordinate type!");
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
                for (int x = 0; x < Board.getLen(); x++) { 
                    for (int y = 0; y < Board.getLen(); y++) {
                        
                        Board newGrid = null;
                        try{            
                            newGrid = oldGrid.deepCopy();
                        } catch (Exception e) {
                            System.out.println("Uhoh! deepCopy has errored");
                            System.out.println(e.getStackTrace());
                        }

                        try { 
                            newGrid.placeShip(newShip,Board.coord(x, y),dir);
                            newBoards.add(newGrid);
                        } 
                        catch (InvalidPlacementException e){} 
                        catch(InvalidShipTypeException e){
                            System.out.println("Uhoh! Bad ship type!");
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
                System.out.println("Please enter a valid coordinate in the form \"x,y\" (range is (0,"+(Board.getLen()-1)+") inclusive)");
            } catch (NumberFormatException e){
                System.out.println("Please enter a valid coordinate in the form \"x,y\" (range is (0,"+(Board.getLen()-1)+") inclusive)");
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
                System.out.println("Please enter a valid coordinate in the form \"x,y\" (range is (0,"+(Board.getLen()-1)+") inclusive)");
            } catch (NumberFormatException e){
                System.out.println("Please enter a valid coordinate in the form \"x,y\" (range is (0,"+(Board.getLen()-1)+") inclusive)");
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
    private static List<Ship> parseSettings(String[] args) {
        List<Ship> fleet = new ArrayList<>(); 
        int boardSize = 5; // default
       
        if (args.length == 0) {
            fleet.add(new Ship(3, '3'));
            fleet.add(new Ship(2, '2')); 
            // System.out.println("Defult game: len "+boardSize+" and ships "+fleet.toString());
        }

        else if(args.length == 2){ // the input is like this "[3:a, 2:z, 4:w]" (where its length:char)
        
            try {
                boardSize = Integer.parseInt(args[0]);
            } catch (NumberFormatException e) {
                System.out.println("! Invalid input args ! (size must be an int)");
                System.exit(0);
            }

            try {
                String[] ships = args[1].replace("[", "").replace("]", "").split(",");
                for (String s : ships) {
                    s = s.replaceAll("\\s", ""); // remove all whitespace
                    String[] ship = s.split(":"); // split into length and char
                    fleet.add(new Ship(Integer.parseInt(ship[0]), ship[1].charAt(0)));
            }
            } catch (Exception e) {
                System.out.println("! Invalid input args !");
                System.exit(0);
            }
            // System.out.println("Custom game: len "+boardSize+" and ships "+fleet.toString());

        }
        else{
            System.out.println("! Invalid input args ! (must be 2 args: size and [fleet])");
            System.exit(0);
        }

        Board.setBoardSize(boardSize);

        return fleet;
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
            System.out.println("Error: File not saved?");
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
            System.out.println("Error: File not saved");
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
                        System.out.println("Something went wrong in the byte conversion!");
                    }
                }
                inputList.add(shipBytes);

            }

            br.close();
        }
        catch (FileNotFoundException e) {
            System.out.println("Uhoh! " + filename + " doesn't exist.");
        }
        catch (IOException e) {
            System.out.println("Uhoh! Something went wrong on the read.");
        }
        return inputList;
    }
    
    
}
