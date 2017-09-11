package ownsecurity;

import org.bouncycastle.jce.provider.BouncyCastleProvider;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.Path;
import java.io.*;
import javax.crypto.Cipher;
import java.security.NoSuchAlgorithmException;
import java.security.Key;
import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;
import javax.crypto.CipherOutputStream;

class ObjectSecurity {
    private String testFile = "text";
    private byte[] data = null;
    
    public static void main(String[] args) {
        new ObjectSecurity().start(args);
    }
    
    private void readFile(String filename) {
        Path file = Paths.get(filename);
        
        try {
            data = Files.readAllBytes(file);
        } catch(IOException exception) {
            System.out.println("[ERROR] Could not read file.");
        }
    }
    
    /*
    private void createRSAKeys() {
        String publicKeyFilename = "public_key", privateKeyFilename = "private_key";
        
        GenerateRSAKeys generateKeys = new GenerateRSAKeys();
        generateKeys.generate(publicKeyFilename, privateKeyFilename);
        
        try {
            Security.addProvider(new org.bouncycastle.jce.provider.BouncyCastleProvider());
            
            KeyProviderGenerator generator = KeyPairGenerator.getInstance("RSA", "BC");
            BASE64Encoder b64 = new BASE64Encoder();
            
            SecureRandom random = createFixedRandom();
            generator.initialize(1024, random);
            
            KeyPair pair = generator.generateKeyPair();
            Key publicKey = pair.getPublic();
            Key privateKey = pair.getPrivate();
            
            System.out.println("publicKey: " + b64.encode(publicKey.getEncoded()));
            System.out.println("privateKey: " + b64.encode(privateKey.getEncoded()));
        } catch(Exception exception) {
        }
    }
    */
    
    private void encryptFile() {
        Cipher cipher = null;
        KeyGenerator generator = null;
        
        try {
            generator = KeyGenerator.getInstance("AES", new BouncyCastleProvider());
            cipher = Cipher.getInstance("AES", new BouncyCastleProvider());
        } catch(Exception exception) {
        }
        
        generator.init(128);
        SecretKey key = generator.generateKey();
        
        try {
            ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
            CipherOutputStream cipherOutputStream = new CipherOutputStream(outputStream, cipher);
            cipherOutputStream.write(data);
            cipherOutputStream.flush();
            cipherOutputStream.close();
            byte[] encrypted = outputStream.toByteArray();
            
            FileOutputStream fileStream = new FileOutputStream("text_encoded");
            fileStream.write(encrypted);
            fileStream.close();
        } catch(Exception exception) {
        }
    }
    
    private void start(String[] args) {
        //createRSAKeys();
        readFile(testFile);
        encryptFile();
        //sendFile()
    }
}