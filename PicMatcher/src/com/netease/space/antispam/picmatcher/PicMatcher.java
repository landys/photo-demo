package com.netease.space.antispam.picmatcher;

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
     * Add the image to the established index.
     * 
     * @param fileName
     *            the image file to be added to the index.
     * @param id
     *            the id of the file.
     */
    void addToIndex(final String fileName, final long id);

    /**
     * Query the image file is in the index library.
     * 
     * @param fileName
     *            the image file to be queried.
     * @return id of the matched file in the index; otherwise -1.
     */
    long query(final String fileName);
}
