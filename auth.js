// ============================================================
//  NOVA BANK - shared auth + role helpers
//  Import this from any page that needs login / guarding.
// ============================================================
import { auth, db, ROLES, COL } from "./firebase-config.js";
import {
  signInWithEmailAndPassword,
  createUserWithEmailAndPassword,
  signOut,
  onAuthStateChanged,
} from "https://www.gstatic.com/firebasejs/10.12.2/firebase-auth.js";
import {
  doc, getDoc, setDoc, serverTimestamp,
} from "https://www.gstatic.com/firebasejs/10.12.2/firebase-firestore.js";

// ---- read the role profile document for a logged-in uid ----
export async function getProfile(uid) {
  const snap = await getDoc(doc(db, COL.USERS, uid));
  return snap.exists() ? snap.data() : null;
}

// ---- sign in, then return the profile (with role) ----
export async function login(email, password) {
  const cred = await signInWithEmailAndPassword(auth, email.trim(), password);
  const profile = await getProfile(cred.user.uid);
  if (!profile) {
    // logged in to Auth but no role doc -> incomplete account
    await signOut(auth);
    throw new Error("No profile found for this account. Contact the admin.");
  }
  return { uid: cred.user.uid, ...profile };
}

// ---- create a customer self-registration account ----
export async function registerCustomer({ firstName, lastName, email, phone, password }) {
  const cred = await createUserWithEmailAndPassword(auth, email.trim(), password);
  const uid = cred.user.uid;
  const profile = {
    uid,
    firstName: firstName.trim(),
    lastName: lastName.trim(),
    email: email.trim(),
    phone: (phone || "").trim(),
    role: ROLES.CUSTOMER,
    status: "active",
    createdAt: serverTimestamp(),
  };
  await setDoc(doc(db, COL.USERS, uid), profile);
  return { uid, ...profile };
}

export function logout() {
  return signOut(auth);
}

// ---- page guard: ensure a user is logged in and (optionally) has a role ----
// usage:  guard(["admin","employee"]).then(profile => { ... })
export function guard(allowedRoles = null) {
  return new Promise((resolve) => {
    onAuthStateChanged(auth, async (user) => {
      if (!user) {
        window.location.href = "loginPage.html";
        return;
      }
      const profile = await getProfile(user.uid);
      if (!profile) {
        await signOut(auth);
        window.location.href = "loginPage.html";
        return;
      }
      if (allowedRoles && !allowedRoles.includes(profile.role)) {
        // logged in but not allowed here -> send to their own home
        window.location.href = homeFor(profile.role);
        return;
      }
      resolve({ uid: user.uid, ...profile });
    });
  });
}

// ---- where each role lands after login ----
export function homeFor(role) {
  if (role === ROLES.ADMIN || role === ROLES.EMPLOYEE) return "dashboard.html";
  return "my-account.html"; // customer self-service
}
