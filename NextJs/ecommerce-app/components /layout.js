import Nav from "@/components /Nav"
import { useSession, signIn, signOut } from "next-auth/react"



export default function Layout({children}) {
  const { data: session } = useSession()
  if(!session) {
    return (
      <div className= "loginScreen"> 
      <div className="googleDiv">
        <button  onClick= {() => signIn('google')}className="googleButton"> <b>Login with Googe</b> </button>
        
        </div> 
    </div>
    )
  }
  return (
    <div className="page">
      <div id="toolbar" className="bg-fuchsia-800 min-h-screen flex">
        <Nav />
        <div id="content" className="bg-white w-screen flex-grow mr-2 mb-2 px-2 py-1 rounded-lg">
          {children}
        </div>
      </div>
    </div>
    
  )
}
