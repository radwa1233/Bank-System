// ============================================================
//  NOVA BANK - Firebase configuration & shared helpers
//  Loaded as an ES module from the browser (no build step).
// ============================================================
import { initializeApp } from "https://www.gstatic.com/firebasejs/10.12.2/firebase-app.js";
import {
  getAuth,
  setPersistence,
  browserLocalPersistence,
} from "https://www.gstatic.com/firebasejs/10.12.2/firebase-auth.js";
import { getFirestore } from "https://www.gstatic.com/firebasejs/10.12.2/firebase-firestore.js";

const firebaseConfig = {
  apiKey: "AIzaSyD37txEkGz5M1mw-f3vml38pBgDHADNk3U",
  authDomain: "bank-system-1bb32.firebaseapp.com",
  projectId: "bank-system-1bb32",
  storageBucket: "bank-system-1bb32.firebasestorage.app",
  messagingSenderId: "101329855718",
  appId: "1:101329855718:web:d4cf89c6accaed6fd38727",
  measurementId: "G-NDVLHZ7FY7",
};

const app = initializeApp(firebaseConfig);

export const auth = getAuth(app);
export const db = getFirestore(app);

// keep the user logged in across tabs / refreshes
setPersistence(auth, browserLocalPersistence).catch(() => {});

// ---- collection / role constants used across the app ----
export const ROLES = { ADMIN: "admin", EMPLOYEE: "employee", CUSTOMER: "customer" };
export const COL = {
  USERS: "users",        // login accounts (admin / employee / customer) keyed by auth uid
  CLIENTS: "clients",    // the person (bank customer profile)
  ACCOUNTS: "accounts",  // bank accounts belonging to a client
  TX: "transactions",    // deposits / withdrawals / transfers
  CURRENCIES: "currencies",
};
