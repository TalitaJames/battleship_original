public class PrimitiveBoard{
    private int[][] board;
    boolean isBad;

    /**
     * Updates:
     * 2d bool array
     * for each ship
     * check ship position (OR over the subarray that the ship will be)
     * if true (intersection) return
     * else add ship
     * end for
     * 
     * the int method (with bigint)
     * make a board
     * add a ship
     * make new board (add new ship)
     * 
     * AND boards if 0 (no intersection)
     *      add them to add the ship to board
     * if != 0, (intersection), early return
     */




    // constructor that takes a Board
    private PrimitiveBoard(){
        this.board = new int[Board.getLength()][Board.getLength()];
        isBad=false;
    }    

    // constructor that takes an array of Bytes
    public static PrimitiveBoard makePrimitiveBoard(Byte[] shipCodes, Ship[] fleet) {
            // throws InvalidPlacementException, InvalidShipTypeException, InvalidPositionException, InvalidIntersectionException {
    
        PrimitiveBoard primBoard = new PrimitiveBoard();   

        for (int i = 0; i < shipCodes.length; i++) {            
            //decoding each byte
            byte encodedShip = shipCodes[i];
            boolean dir = (encodedShip % 2 != 0); // if odd, then true true
                
            int uint = encodedShip & 0xff; // unsign it
            uint>>=1; // get rid of directional info

            int x= (int) Math.floor(uint/10); // undoes encoding in the form of x*10+y
            int y= uint % 10;
            
            primBoard.placeShip(fleet[i], x, y, dir);
        }

        // now that all the ships are in, check if any are overlapping (ie a value in array>1)
        for(int y=0; y < Board.getLength(); y++){
            for(int x=0; x < Board.getLength(); x++){
                if(primBoard.getXY(x,y)>1){
                    primBoard.setIsBad(); //throw new InvalidIntersectionException("A primative intersection!");
                }
            }
        }

        return primBoard;
    }
    
    private void placeShip(Ship ship, int x, int y, boolean direction){ // direction horizontal (x) = true
            // throws InvalidPlacementException, InvalidShipTypeException, InvalidPositionException {
               
        if (ship == null) isBad=true; //throw new InvalidShipTypeException("Null ship");

        if(direction && (Board.getLength() < (x+ship.getLength()-1))) isBad=true;//throw new InvalidPositionException("Bad possition: X overhang");
        else if (!direction && (Board.getLength() < (y+ship.getLength()-1))) isBad=true;//throw new InvalidPositionException("Bad possition: Y overhang");

        // add the ship
        for (int offset = 0; offset < ship.getLength(); offset++) { 
            try{
                if(direction){
                    board[y][x+offset]+=1;
                } else{
                    board[y+offset][x]+=1;
                }
            } catch (ArrayIndexOutOfBoundsException e){
                // throw new InvalidPositionException("Array out of bounds (adding)");
                isBad=true;
            }
        }

    }

    private int getXY(int x, int y){
        return board[y][x];
    }
    private void setIsBad(){
        isBad=true;
    }

    public boolean getIsBad(){
        return isBad;
    }

    // toString (copy from Board)
}