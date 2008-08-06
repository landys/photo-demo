package com.picMatcher;

/**
 * This interface includes methods to setup index and query an image.
 * 
 * @author tony
 */
public interface PicMatcher {
    /**
     * Use the image library under the certain path to set up index.
     * 
     * @param path
     *            the path of the image library to be set up index.
     */
    void setupIndex(final String path);

    /**
     * Query the image file is in the index library.
     * 
     * @param fileName
     *            the image file to be queried.
     * @return true if the file is in the index library; otherwise false.
     */
    boolean query(final String fileName);
}
