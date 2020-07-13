import java.util.Scanner;

public class BankingApplication {

    public static void main(String[] args) {
      BankAccount obj1 = new BankAccount("xyz", "BA0001");
      obj1.showMenu();
    }// Main

}// BankingApplication class

class BankAccount {
    int balance;
    int previousTransaction;
    String customerName;
    String customerID;

    BankAccount(String cname, String cID) { // Constructor
      customerName = cname;
      customerID = cID;
    }

    void deposit(int amount) {
        if (amount != 0) {
          balance = balance + amount;
          previousTransaction = amount;
        }
    }

    void withdraw(int amount) {
        if (amount != 0) {
          balance = balance - amount;
          previousTransaction = -amount;
        }
    }

    void getPreviousTransaction() {
      if (previousTransaction > 0) {
        System.out.println("Deposited: $" + previousTransaction);
      } else if (previousTransaction < 0) {
        System.out.println("Withdrew: $" + previousTransaction);
      } else {
        System.out.println("No transaction has occured.");
      }
    }

    void showMenu() {
      char option = '\0';
      Scanner scan = new Scanner(System.in);

      System.out.println("Welcome " + customerName);
      System.out.println("Your ID is " + customerID);
      System.out.println();
      System.out.println("A. Check Balance");
      System.out.println("B. Deposit");
      System.out.println("C. Withdraw");
      System.out.println("D. Previous Transaction");
      System.out.println("E. Exit");

      do {
        System.out.println("===========================================");
        System.out.println("Enter an option.");
        System.out.println("===========================================");
        System.out.println();
        option = scan.next().charAt(0);

        switch(option) {
          case 'A':
          System.out.println("---------------------------------");
          System.out.println("Balance = $" + balance);
          System.out.println("---------------------------------");
          System.out.println();
          break;

          case 'B':
          System.out.println("---------------------------------");
          System.out.println("Enter an amount to deposit");
          System.out.println("---------------------------------");
          int amount = scan.nextInt();
          deposit(amount);
          System.out.println();
          break;

          case 'C':
          System.out.println("---------------------------------");
          System.out.println("Enter an amount to withdraw");
          System.out.println("---------------------------------");
          int amount2 = scan.nextInt();
          withdraw(amount2);
          System.out.println();
          break;

          case 'D':
          System.out.println("---------------------------------");
          getPreviousTransaction();
          System.out.println("---------------------------------");
          System.out.println();
          break;

          case 'E':
          System.out.println("*******************************************");
          System.out.println();
          break;

          default:
          System.out.println("Invalid Option! Please enter again");
          break;
        }
      }while (option != 'E');

      System.out.println("Thank you for using this Banking Application.");
    }
} // BankAccount Class
