/*
 * JBoss, Home of Professional Open Source
 * Copyright 2008, Red Hat, Inc., and others contributors as indicated
 * by the @authors tag. All rights reserved.
 * See the copyright.txt in the distribution for a
 * full listing of individual contributors.
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU Lesser General Public License, v. 2.1.
 * This program is distributed in the hope that it will be useful, but WITHOUT A
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public License,
 * v.2.1 along with this distribution; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 */
package org.jboss.blacktie.btadmin;

import java.io.File;
import java.io.IOException;
import java.net.MalformedURLException;

import javax.management.MalformedObjectNameException;

import org.apache.log4j.BasicConfigurator;
import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;
import org.jboss.blacktie.jatmibroker.core.conf.ConfigurationException;

/**
 * Launcher for the btadmin tool.
 * 
 * @author tomjenkinson
 */
public class Launcher {
	private static Logger log = LogManager.getLogger(Launcher.class);

	public static void main(String[] args) {
		if (System.getProperty("log4cxx.configuration") == null
				&& !new File("log4cxx.properties").exists()
				&& !new File("log4j.xml").exists()) {
			BasicConfigurator.configure();
		}

		boolean commandSuccessful = false;
		try {
			CommandHandler commandHandler = new CommandHandler();
			commandSuccessful = commandHandler.handleCommand(args);
		} catch (MalformedURLException e) {
			log.error("JMXURL was incorrect: " + e.getMessage(), e);
		} catch (IOException e) {
			log
					.error("Could not connect to mbean server: "
							+ e.getMessage(), e);
		} catch (MalformedObjectNameException e) {
			log.error("MBean name was badly structured: " + e.getMessage(), e);
		} catch (NullPointerException e) {
			log.error("MBean name raised an NPE: " + e.getMessage(), e);
		} catch (ConfigurationException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}

		// Check whether we need to exit the launcher
		if (commandSuccessful) {
			log.trace("Command was successfull");
		} else {
			log.trace("Command failed");
			System.exit(-1);
		}
	}
}
