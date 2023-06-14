public class Ship {
    // Where it starts
    // how long it is
    // which direction
    private final int startPosX;
    private final int startPosY;
    private final int length;
    private final boolean direction; // true = horizontal (->) false = vertical (V)
    private final char symbol;
    

    private Ship(int startPosX, int startPosY, int length, boolean direction, char symbol) {
        this.startPosX=startPosX;
        this.startPosY=startPosY;
        this.length=length;
        this.direction=direction;
        this.symbol=symbol;
    }

    // direction (horiz = true, vert = false)
    public static Ship createShip(int startPosX, int startPosY, int shipLength,
                                    boolean direction, char symbol, int boardLength) throws InvalidPositionException{
        
        if(0>startPosX || startPosX>=boardLength || startPosY<0 || startPosY>=boardLength){
            throw new InvalidPositionException("Out of bounds");
        }
        else if(direction && !((boardLength-shipLength)>=startPosX)){
            throw new InvalidPositionException("Bad start X");
        }
        else if(!direction && !((boardLength-shipLength)>=startPosY)){
            throw new InvalidPositionException("Bad start Y");
        }

        return new Ship(startPosX, startPosY, shipLength, direction,symbol);
    }

    @Override
    public String toString(){
        return "Ship at ("+startPosX+", "+startPosY+") len: "+length+" dir: "+direction;
    }

    //  all the getters
    public int getLength() {
        return length;
    }
    
    public boolean getDirection() {
        // because i can't remind myself enough
        // horizontal = true  (x axis)
        //   vertical = false (y axis)
        return direction; 
    } 

    public int getStartPosX() {
        return startPosX;
    }

    public int getStartPosY() {
        return startPosY;
    }

    public char getSymbol() {
        return symbol;
    }
    // end of the getters


}